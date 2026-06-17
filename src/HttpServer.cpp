#include "HttpServer.h"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string.h>

//8080 这个数字，首先是交给了 HttpServer 的构造函数 HttpServer(int port)，但是C++规定父类构造函数要在子类之前
//所以子类要将端口号向上传递给父类
//利用成员初始化列表，把 port 扔给父类的构造函数去初始化底层的 server_fd
//子类还能继承父类的构造函数，所以在子类的构造函数的初始化列表里把port传给父类的构造函数
HttpServer::HttpServer(int port):EpollServer(port)
{
    logger.info("[HttpServer] HTTP 应用层已挂载，并发路由就绪！");
}

//注册路由接口实现：当外面调用 server.add_route 时，实际上是 router工作
void HttpServer::add_route(const std::string &path,HandleFunc handler)
{
    router.add_route(path,handler);
}
//重写父类的handle_client_data函数
void HttpServer::handle_client_data(int client_fd)
{
    char buffer[4096];
    memset(buffer,0,sizeof(buffer));
    //读取浏览器发送来的字节流
    ssize_t bytes_read=recv(client_fd,buffer,sizeof(buffer)-1,0); 
    if(bytes_read>0)
    {
        
        //将底层 C 风格的字符数组，转化为 C++ 的字符串，放入 HttpRequest 中解析
        //注：使用深拷贝调用string的构造函数，将栈内存的buffer在堆内存申请一块刚好容纳 bytes_read 大小的新内存，然后存入
        //浅拷贝 (Shallow Copy)： 仅仅拷贝内存地址，改了其中一个，另一个也会变
        //深拷贝(Deep Copy)： 连同内存里的数据一起完整复制一份
        std::string raw_request(buffer,bytes_read);
        thread_pool.submit([this,client_fd,raw_request]()
            {
                HttpRequest req(raw_request);
                //准备一个空的res，用于填充响应报文的数据
                //基础数据类型创建后不初始化里面是脏数据
                //类对象创建后强制调用默认构造函数，所以是空的不用初始化
                HttpResponse res;
                //路由分发
                this->router.handle_request(req,res);
                //序列化打包成响应报文的格式
                std::string final_packet=res.to_string();
                //发送回浏览器
                ssize_t bytes_sent=send(client_fd,final_packet.c_str(),final_packet.length(),0);
                if(bytes_sent==-1)
                {
                    //非阻塞 Socket 遇到发送缓冲区满
                    if(errno==EAGAIN||errno==EWOULDBLOCK)
                    {
                        this->logger.error("[发送缓冲满] fd: " + std::to_string(client_fd) + "，报文未能一次性发完");
                    }
                    else
                    {
                        //网络错误
                        this->logger.error("[发送失败] fd: " + std::to_string(client_fd) + " 错误: " + std::string(strerror(errno)));
                    }
                }
                close(client_fd);
                //日志记录:提取req和res的信息
                std::string log_msg = "[访问记录] " + req.get_method() + " " + req.get_path() + 
                                  " | 状态码: " + std::to_string(res.get_status_code()) +
                                  " | fd: " + std::to_string(client_fd);
                if(res.get_status_code()==404||res.get_status_code()==500)
                {
                    this->logger.error(log_msg);
                }
                else
                {
                    this->logger.info(log_msg);
                }
            });

    }
    //合法离开
    else if(bytes_read==0)
    {
        logger.info("[断开] 客户端 fd: " + std::to_string(client_fd) + " 已主动下线。");
        close(client_fd);
    }

    //异常与非阻塞中断
    else
    {
        if(errno==EAGAIN||errno==EWOULDBLOCK)
        {
            return;
        }
        else
        {
            logger.error("[异常] 客户端 fd: " + std::to_string(client_fd) + " 发生网络异常断开。");
            close(client_fd);
        }
    }
}
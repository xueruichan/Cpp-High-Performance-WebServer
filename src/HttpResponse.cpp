//HTTP 协议的官方定义里，响应头和请求头,全都是标准的键值对
#include "HttpResponse.h"
//构造函数
//int（内置基础类型）：如果不手动初始化，它在内存里是一个随机的垃圾数字
//headers (std::map) 和 body (std::string) 是标准库的类对象。即使不初始化，C++ 也会在底层自动调用它们的无参默认构造函数，是干净的
HttpResponse::HttpResponse():status_code(200)
{

}
void HttpResponse::set_status_code(int code)
{
    status_code=code;
}
void HttpResponse::set_header(const std::string &key,const std::string &value)
{
    //构造map中的键值对,不存在则创建，存在则覆盖修改
    headers[key]=value;
}
//body内容就属于前端    
void HttpResponse::set_body(const std::string &content)
{
    body=content;
}
//状态码查询对应状态信息
std::string HttpResponse::get_status_message(int code) const
{
    switch(code)
    {
        case 200: return "OK";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        default: return "Unknown";
    }
}
//序列化生成响应报文
std::string HttpResponse::to_string() const
{
    std::string response;
    //拼接状态行
    response+="HTTP/1.1"+std::to_string(status_code)+" "+get_status_message(status_code)+"\r\n";
    //拼接响应头
    response+="Content-Length: "+std::to_string(body.length())+"\r\n";
    //遍历用户设置的其他响应头（如 Content-Type, Connection 等）
    //基于范围的for循环，相当于容器遍历器，须依托于一个真正的“集合”（比如数组、std::vector 动态数组、std::map 字典等）
    //for ( 元素类型 变量名 : 要遍历的容器 )
    //for (const auto& x : vec)：只读引用
    for(const auto &pair:headers)
    {
        response+=pair.first+": "+pair.second+"\r\n";
    }
    //拼接空行，代表响应头结束
    response+="\r\n";

    //拼接响应体
    response+=body;
    return response;
}
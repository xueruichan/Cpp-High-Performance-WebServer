//读取静态资源
//Router 的核心职责仅仅是分发： 它的任务是拿着 path 去找对应的处理逻辑
#include "StaticFileServer.h"
#include <fstream>
#include <iostream>

StaticFileServer::StaticFileServer(const std::string &directory):base_dir(directory){}

bool StaticFileServer::serve_file(const HttpRequest &req,HttpResponse &res) const
{
    std::string path=req.get_path();
    //默认主页
    if(path=="/")
    {
        path="/index.html";
    }
    //拼接真实物理地址
    std::string filepath=base_dir+path;
    //二进制打开文件
    std::ifstream file(filepath,std::ios::binary);
    if(!file.is_open())
    {
        return false;
    }
    //二进制模式打开文件
    //file与content的关系：
    //file：
    //文件输入流:不是文件数据本身，它是一个系统接口
    //C++ 只是向操作系统申请打开了一条连接物理硬盘中该文件的“通道”。此时，文件的字节内容依然安静地躺在磁盘（ROM）上，并没有进入程序的运行内存（RAM）
    //content：在RAM中开辟空间，通过 file 这个系统通道，向硬盘发起 I/O 读取指令，塞进 content
    std::string content((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
    file.close();

    //填充res对象
    res.set_status_code(200);
    res.set_header("Content-Type",get_mime_type(filepath));
    res.set_header("Connection",req.get_header("Connection"));
    res.set_body(content);
    return true;
}
std::string StaticFileServer::get_mime_type(const std::string& filepath) const 
{
    size_t dot_pos = filepath.find_last_of('.');
    if (dot_pos == std::string::npos) return "text/plain";

    std::string ext = filepath.substr(dot_pos);
    if (ext == ".html" || ext == ".htm") return "text/html";
    if (ext == ".css") return "text/css";
    if (ext == ".js") return "application/javascript";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".json") return "application/json";
    return "text/plain";
}
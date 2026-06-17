#pragma once
#include <string>
#include <map>
class HttpResponse
{
private:
    //存储状态码，响应头，响应体
    int status_code;
    std::map<std::string,std::string> headers;
    std::string body;
    //根据状态码获取对应的状态信息描述（属于状态行）
    std::string get_status_message(int code) const;
public:
    HttpResponse();
    ~HttpResponse()=default;
    //写入接口：供外部填充数据
    void set_status_code(int code);
    void set_header(const std::string &key,const std::string &value);
    void set_body(const std::string &content);
    int get_status_code() const {return status_code;}

    //将对象里的数据封装成响应报文的格式
    std::string to_string() const;
};
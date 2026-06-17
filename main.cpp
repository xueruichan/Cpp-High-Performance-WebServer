#include "HttpServer.h"
#include <iostream>

int main() {
    // 1. 实例化 HttpServer，监听 8080 端口
    HttpServer server(8080);

    // 2. 注册动态 API (使用 Lambda 表达式)
    server.add_route("/api/ping", [](const HttpRequest& req, HttpResponse& res) {
        res.set_status_code(200);
        res.set_header("Content-Type", "application/json");
        res.set_body("{\"status\": \"success\", \"message\": \"pong!\"}");
    });

    // 3. 启动服务器 (调用的是父类 EpollServer 的 start 函数)
    // 此时主线程会死循环在 epoll_wait 中，等待连接和数据
    server.start();

    return 0;
}
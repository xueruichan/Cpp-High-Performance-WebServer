#pragma once
#include <vector>
#include <thread>
#include <functional>

#include "BlockingQueue.h"

class ThreadPool
{
private:
    //创建工作线程数组
    std::vector<std::thread> workers;
    BlockingQueue<std::function<void()>> taskQueue;
public:
    ThreadPool(const ThreadPool&)=delete;
    ThreadPool& operator=(const ThreadPool&)=delete;
    ThreadPool(int num_threads);
    void submit(std::function<void()> task);
    ~ThreadPool();
};
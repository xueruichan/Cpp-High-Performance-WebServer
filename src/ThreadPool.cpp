#include "ThreadPool.h"

ThreadPool::ThreadPool(int num_threads)
{
    for(int i=0;i<num_threads;i++)
    {
        //创建指定数量的线程，并执行lambda里面的代码
        //vector的emplace_back
        //emplace_back根本不用接收一个已经建好的 thread 对象。它接收的是构造thread 所需的原材料（也就是那个 lambda 表达式）
        workers.emplace_back([this]()
        {
            //准备一个task变量用于接收任务队列中取出的任务
            std::function<void()> task;
            //具体见BlockingQueue.h的pop函数的定义
            while(taskQueue.pop(task))
            {
                //执行任务
                task();
            }
        }); 
    }
}

void ThreadPool::submit(std::function<void()> task)
{
    //具体见BlockingQueue.h的push函数的定义
    taskQueue.push(task);
}

ThreadPool::~ThreadPool()
{
    //1.停止生产任务并唤醒阻塞队列中的线程
    //具体见BlockingQueue.h的stop函数的定义
    taskQueue.stop();

    //2.结束线程
    for(size_t i=0;i<workers.size();i++)
    {
        if(workers[i].joinable())
        {
            workers[i].join();
        }
    }
}
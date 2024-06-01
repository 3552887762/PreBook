
#ifndef _ASYNCLOGGING_H_
#define _ASYNCLOGGING_H_
#include<atomic>
#include<thread>
#include<memory>
#include<mutex>
#include<condition_variable> 
#include<string>
#include<vector>
using namespace std;
//own
#include"LogFile.hpp"
#include"CountDownLatch.hpp"

namespace tulun
{
    class AsyncLogging
    {
    private:
    	AsyncLogging(const AsyncLogging&) = delete;
        AsyncLogging& operator=(const AsyncLogging&) = delete;
    private:
        void workthreadfunc();      // 工作线程
    private:  // 20:00 shangke
        const int flushInterval_;   // 定期（flushInterval_秒）将缓冲区的数据写到文件中
        std::atomic<bool> running_; // 是否正在运行
        const string basename_;     // 日志filename名字
        const size_t rollSize_;     // 回滚大小
        tulun::CountDownLatch latch_; //倒计时对象
        std::unique_ptr<std::thread> pthread_; // 执行该异步日志记录器的线程
        std::mutex mutex_;                     //
        std::condition_variable cond_;         // 
        std::string currentBuffer_;            // 当前的缓冲区
        std::vector<std::string>  buffers_;    // 数据缓冲区队列
        tulun::LogFile output_;                //定义日志文件对象 
    public:
        //rollSize //回滚大小
        //flushInterval = 3 ; // 刷新间隔,默认值3秒
        AsyncLogging(const string &basename,size_t rollSize,int flushInterval = 3);
        ~AsyncLogging();
        void append(const string &info);
        void append(const char *info,int len);
        void start();
        void stop();
        void flush();
    };
}
#endif

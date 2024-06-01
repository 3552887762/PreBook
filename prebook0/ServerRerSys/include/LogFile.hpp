#ifndef _LOGFILE_H
#define _LOGFILE_H

//C++
#include<string>
#include<mutex>
#include<memory>
using namespace std;
// C
#include<time.h> 
//own
#include"Timestamp.hpp"
#include"AppendFile.hpp"

namespace tulun
{

class LogFile
{
private:
    const string basename_;   //日志文件名basename 
    const size_t rollSize_;   //日志文件存储数据量达到rollSize时,生成一个新文件
    const int flushInterval_; //日志写入间隔时间 秒级
    const int checkEveryN_;   //每调用checkEveryN_次日志写，就滚动一次日志
    int   count_;             // 写文件的次数 
private:
    time_t startOfPeriod_;    // 开始写日志的周期，在同一个周期内的日志数据会写入同一个日志文件中
    time_t lastRoll_;         // 最后一次滚动日志的时间 
    time_t lastFlush_;        // 上一次刷新日志文件时间
private:
    std::unique_ptr<std::mutex>  mutex_;
    std::unique_ptr<tulun::AppendFile> file_;
    static const int kRollPerSeconds_ = 60*60*24; //roll 的周期：即60*60*24 = 1day
private:
    //不加锁的append方式
    void append_unlocked(const char* logline, const int len);
    //获取日志文件的名称
    static string getLogFileName(const string& basename, const MicroTimestamp &now);
public:
    LogFile(const string &basename, 	//日志文件basename 
            size_t rollSize,            //日志文件达到rollsize生成一个新文件
            bool threadSafe = true,    // 线程安全控制项, 默认为true. 
                                       //当只有一个后端AsnycLogging和后端线程时, 
                                       //该项可置为false
        	int flushInterval = 3,     //日志写入间隔时间 秒级
            int checkEveryN = 1024);   //每调用checkEveryN_次日志写，就滚动一次日志
    ~LogFile();
    void append(const std::string &info);
    void append(const char *info,const int len);
    void flush();
    bool rollFile();
};

}
#endif 
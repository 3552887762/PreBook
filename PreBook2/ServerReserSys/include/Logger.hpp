
#ifndef _LOGGER_H_
#define _LOGGER_H_
//C++
#include<functional> // function
#include<string>
using namespace std;

//own
#include"LogCommon.hpp"
#include"LogMessage.hpp"

namespace tulun
{

class Logger
{
public:
    // using OutputFunc = void (*)(const char *msg,int len);
    // using FlushFunc = void(*)();
    using OutputFunc = std::function< void (const std::string &msg) >;
    using FlushFunc =  std::function<void()>;
    static OutputFunc output_;
    static FlushFunc  flush_;
public:
   static void setOutput(OutputFunc);
   static void setFlush(FlushFunc);
private:
    LogMessage impl_;         
     // 日志级别
public:
    // [日志级别];[文件名称];[函数名称];[行号]  
    Logger(tulun::LOG_LEVEL level,const string &name,const string &func,const int line);
    ~Logger();
    LogMessage & stream();
    static void setLogLevel(LOG_LEVEL level);
    static LOG_LEVEL getLogLevel();
private:
    static LOG_LEVEL s_level_;
};  

#define LOG_TRACE if(tulun::Logger::getLogLevel()<= tulun::LOG_LEVEL::TRACE) \
    tulun::Logger(tulun::LOG_LEVEL::TRACE,__FILE__,__func__, __LINE__).stream()

#define LOG_DEBUG if(tulun::Logger::getLogLevel()<= tulun::LOG_LEVEL::DEBUG) \
    tulun::Logger(tulun::LOG_LEVEL::DEBUG,__FILE__,__func__, __LINE__).stream()

#define LOG_INFO if(tulun::Logger::getLogLevel()<= tulun::LOG_LEVEL::INFO) \
    tulun::Logger(tulun::LOG_LEVEL::INFO, __FILE__, __func__,__LINE__).stream()

#define LOG_WARN tulun::Logger(tulun::LOG_LEVEL::WARN, __FILE__ ,__func__,__LINE__).stream()

#define LOG_ERROR tulun::Logger(tulun::LOG_LEVEL::ERROR, __FILE__,__func__,__LINE__).stream()

#define LOG_FATAL tulun::Logger(tulun::LOG_LEVEL::FATAL, __FILE__,__func__,__LINE__).stream()



}
#endif 
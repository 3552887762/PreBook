

#ifndef __LOGCOMMON_H
#define __LOGCOMMON_H
namespace  tulun
{
	class copyable
	{
	};

	class nocopyable
	{
	public:
		nocopyable(const nocopyable&) = delete;
		nocopyable& operator=(const nocopyable&) = delete;
	public:
		nocopyable() {}
		~nocopyable() {}
	};
    
    static const int SMALL_BUF_LEN  =  128;
    static const int MEDIAN_BUF_LEN =  512;
    static const int LARGE_BUF_LEN  =  1024;

    enum class LOG_LEVEL
    {
        TRACE = 0, // 跟踪，追踪
        DEBUG,	   // 调试
        INFO,	   // 信息 
        WARN,	   // 警告
        ERROR,	   // 错误
        FATAL,	   // 致命错误
        NUM_LOG_LEVELS, // 日志级别数
    };

    static const char *LLtoStr[]=
    {
        "TRACE",  // 0
        "DEBUG",  // 1
        "INFO",
        "WARN",
        "ERROR",
        "FATAL",
        "NUM_LOG_LEVELS",
    };
}
#endif
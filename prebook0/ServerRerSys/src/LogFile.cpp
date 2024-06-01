
#include"../include/LogFile.hpp"
#include<sys/types.h>
#include<unistd.h>

namespace tulun
{
//const string basename_;   //日志文件名basename 
//const size_t rollSize_;   //日志文件存储数据量达到rollSize时,生成一个新文件
//const int flushInterval_; //日志写入间隔时间 秒级
//const int checkEveryN_;   //每调用checkEveryN_次日志写，就滚动一次日志
//int   count_;             // 写文件的次数 

//time_t startOfPeriod_;    // 开始写日志的周期，在同一个周期内的日志数据会写入同一个日志文件中
//time_t lastRoll_;         // 最后一次滚动日志的时间 
//time_t lastFlush_;        // 上一次刷新日志文件时间
//
//std::unique_ptr<std::mutex>  mutex_;
//std::unique_ptr<tulun::AppendFile> file_;
//static const int kRollPerSeconds_ = 60*60*24; //roll 的周期：即60*60*24 = 1day

pid_t pid()
{
    return ::getpid();
}
const string hostname()
{
    char buff[256]={0};
    if(::gethostname(buff,sizeof(buff)) == 0)
    {
        buff[sizeof(buff)-1] = '\0';
        return buff;
    }
    else
    {
        return "unknownhost";
    }
}
//不加锁的append方式
//append_unlocked 会先将log消息写入file_文件，之后再判断是否需要滚动日志文件；
//如果不滚动，就根据append_unlocked的调用次数和时间，确保
// 1）一个log文件超时（默认1天），就创建一个新的；
//2）flush文件操作，不会频繁执行（默认间隔3秒）。
void LogFile::append_unlocked(const char* logline, const int len)
{
    file_->append(logline,len); // ERROR 2day 345
    if(file_->writtenBytes() > rollSize_)
    {
        rollFile();
    }
    else
    {
        count_++;
        if(count_ >= checkEveryN_)
        {
            count_ = 0;
            time_t now = ::time(nullptr);//
            time_t thisPeriod = now /kRollPerSeconds_ * kRollPerSeconds_;
            if(thisPeriod != startOfPeriod_)
            {
                rollFile();
            }
            else if(now - lastFlush_ > flushInterval_)
            {
                lastFlush_ = now;
                file_->flush();// buffer ==> io
            }
        }
    }
}
//获取日志文件的名称
std::string LogFile::getLogFileName(const string& basename, const MicroTimestamp &now)
{
    std::string filename;
    filename.reserve(basename.size()+128);
    filename = basename;
    filename += now.toFormattedFile();//2023/12/23-12:23:45.23455Z
    filename +=tulun::hostname();
    char pidbuff[128]={0};
    snprintf(pidbuff,sizeof(pidbuff),"%d",tulun::pid());
    filename += pidbuff;
    filename +=".log";
    return filename;
}

LogFile::LogFile(const string &basename, size_t rollSize, 
                 bool threadSafe, int flushInterval, int checkEveryN)   
    :basename_(basename),
    rollSize_(rollSize), // 1G
    flushInterval_(flushInterval),
    checkEveryN_(checkEveryN),
    count_(0),
    mutex_(threadSafe ? new mutex{}:nullptr),
    startOfPeriod_(0),
    lastRoll_(0),
    lastFlush_(0)   
{
    rollFile();
}

LogFile::~LogFile()
{

}
void LogFile::append(const std::string &info)
{
    append(info.c_str(),info.size());
}
void LogFile::append(const char *info,const int len)
{
    if(mutex_)
    {
        std::unique_lock<std::mutex> _lock(*mutex_);
        append_unlocked(info,len);
    }
    else
    {
        append_unlocked(info,len);
    }
}
void LogFile::flush()
{
    if(mutex_)
    {
        std::unique_lock<std::mutex> _lock(*mutex_);
        file_->flush();
    }
    else
    {
        file_->flush();
    }
} // 21:16 shangke 
bool LogFile::rollFile()
{
    MicroTimestamp now(MicroTimestamp::now());
    string filename = getLogFileName(basename_,now);
    time_t start = (now.secondsSinceEpoch()/kRollPerSeconds_)*kRollPerSeconds_;
    if(now.secondsSinceEpoch() > lastRoll_)
    {
        lastRoll_ = now.secondsSinceEpoch(); // 1970,1,1 0:0:0
        lastFlush_ = now.secondsSinceEpoch();
        startOfPeriod_ = start;
        //cout<<filename<<endl;
        file_.reset(new tulun::AppendFile(filename));
        return true;
    }
    return false;
}


}
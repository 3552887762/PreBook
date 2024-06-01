
#include"../include/AsyncLogging.hpp"

namespace tulun
{

static const int BufMaxLen = 4096;
static const int BufQueueSize = 16;

AsyncLogging::AsyncLogging(const string &basename,size_t rollSize,int flushInterval)
    :flushInterval_(flushInterval),
    running_(false),
    rollSize_(rollSize),
    pthread_(nullptr),
    output_(basename,rollSize,false),
    latch_(1)
{
    currentBuffer_.reserve(BufMaxLen); //
    buffers_.reserve(BufQueueSize);
}
AsyncLogging::~AsyncLogging()
{
    if(running_)
    {
        stop();
    }
}
void AsyncLogging::append(const string &info)
{
    append(info.c_str(),info.size());
}
void AsyncLogging::workthreadfunc()     // 工作线程
{
    std::vector<std::string> buffersToWrite; //
    latch_.countDown();
    while(running_)
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(buffers_.empty()) // while(buffers_.empty())
            {
                cond_.wait_for(lock,std::chrono::seconds(flushInterval_));
            } 
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_.reserve(BufMaxLen);
            buffersToWrite.swap(buffers_);
            buffers_.reserve(BufQueueSize);
        }

        if(buffersToWrite.size() > 25) // 4096 * 25
        {
            char buff[256];
            snprintf(buff,sizeof(buff),"Dropped log message at larger buffers \n");
            fputs(buff,stderr);
            buffersToWrite.erase(buffersToWrite.begin()+2,buffersToWrite.end());
        }
        for(const auto &buffer: buffersToWrite)
        {
            output_.append(buffer.c_str(),buffer.size());
        }
        buffersToWrite.clear();
    }
    output_.flush();
}

void AsyncLogging::append(const char *info,int len)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(currentBuffer_.size() >= BufMaxLen ||
       currentBuffer_.capacity() - currentBuffer_.size() < len)
    {
        buffers_.push_back(std::move(currentBuffer_));
        currentBuffer_.reserve(BufMaxLen);
    }
    
    currentBuffer_.append(info,len);
    cond_.notify_all();
}
void AsyncLogging::start()
{
    running_ = true;
    pthread_.reset(new std::thread(&AsyncLogging::workthreadfunc,this));
    latch_.wait();
}
void AsyncLogging::stop()
{
    running_ = false;
    cond_.notify_all();
    pthread_->join();
}
void AsyncLogging::flush()
{
    std::vector<std::string> bufferToWrite;
    std::unique_lock<std::mutex> lock(mutex_);
    buffers_.push_back(currentBuffer_);
    bufferToWrite.swap(buffers_);
    for(const auto &buffer: bufferToWrite)
    {
        output_.append(buffer.c_str(),buffer.size());
    }
    output_.flush();
    bufferToWrite.clear();
}

}
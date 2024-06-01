


#include"../include/CountDownLatch.hpp"

namespace tulun
{
    CountDownLatch::CountDownLatch(int count)
        :count_(count)   {}

    void CountDownLatch::wait()
    {
        std::unique_lock<std::mutex> _lock(mutex_);
        while(count_ > 0)
        {
            condition_.wait(_lock);
        }
    }
    void CountDownLatch::countDown()
    {
        std::unique_lock<std::mutex> _lock(mutex_);
        count_-=1;
        if(count_ == 0)
        {
            condition_.notify_all();
        }
    }
    int CountDownLatch::getCount() const
    {
        std::unique_lock<std::mutex> _lock(mutex_);
        return count_;
    }
}

#include"../include/LogCommon.hpp"
#include"../include/Timestamp.hpp"

#include<time.h>
#include<sys/time.h>

namespace tulun
{

MicroTimestamp::MicroTimestamp()
	: microSecondsSinceEpoch_ {0}
{

}
MicroTimestamp::MicroTimestamp(int64_t microSecondsSinceEpoch)
    :microSecondsSinceEpoch_(microSecondsSinceEpoch)
{

}
void MicroTimestamp::swap(MicroTimestamp& that)
{
    std::swap(this->microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
}
        //按不同的两种格式输出时间
std::string MicroTimestamp::toString() const
{
    char buff[SMALL_BUF_LEN] ={0};
    int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    snprintf(buff,sizeof(buff),"%ld .%06ld",seconds,microseconds);
		//snprintf(buff, sizeof(buff), "%" PRId64 ".%06" PRId64 "", seconds, microseconds);g
		// printf("%"PRId64,value);  printf("%ld",value);
	return buff;
}
std::string MicroTimestamp::toFormattedString(bool showMicroseconds) const
{
    char buff[SMALL_BUF_LEN]={0};
    time_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    struct tm tm_time;
    //gmtime_r(&seconds, &tm_time);// 线程安全 localtime_r获取本地时间。

    localtime_r(&seconds,&tm_time);
    snprintf(buff, sizeof(buff), "%4d/%02d/%02d-%02d:%02d:%02d.%ldZ",
					tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
					tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,microseconds);
	return buff;
}
std::string MicroTimestamp::toFormattedFile() const
{
    char buff[SMALL_BUF_LEN]={0};
    time_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    struct tm tm_time;
    //gmtime_r(&seconds, &tm_time);// 线程安全 localtime_r获取本地时间。

    localtime_r(&seconds,&tm_time);
    snprintf(buff, sizeof(buff), "%4d%02d%02d%02d%02d%02d.%ldZ",
					tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
					tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,microseconds);
	return buff;
}
bool MicroTimestamp::valid() const
{
    return microSecondsSinceEpoch_ > 0;
}
		 // for internal usage.
std::int64_t MicroTimestamp::microSecondsSinceEpoch() const 
{
    return microSecondsSinceEpoch_;
}
time_t MicroTimestamp::secondsSinceEpoch() const
{
    return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
}

MicroTimestamp MicroTimestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv,nullptr);// 1970 : 1 1 0 ===> 2023 
    //time();
    int64_t seconds = tv.tv_sec;
    return MicroTimestamp(seconds * kMicroSecondsPerSecond+ tv.tv_usec );
}
MicroTimestamp MicroTimestamp::invalid()
{
    return MicroTimestamp();
}

}
#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__
#include<ctime>         // include<time.h>
#include<string>        // include<cstring> 
#include"LogCommon.hpp"
namespace tulun
{
    class MicroTimestamp : public tulun::copyable
	{
	private:
		std::int64_t microSecondsSinceEpoch_;// 1970 .....2023 
	public:
		MicroTimestamp();
		explicit MicroTimestamp(int64_t microSecondsSinceEpoch);
		void swap(MicroTimestamp& that);
        //按不同的两种格式输出时间
		std::string toString() const;
		std::string toFormattedString(bool showMicroseconds = true) const;
		std::string toFormattedFile() const;
        bool valid() const;
		 // for internal usage.
		std::int64_t microSecondsSinceEpoch() const ;
		time_t secondsSinceEpoch() const;
	public:
		static MicroTimestamp now();
		static MicroTimestamp invalid();
		static const int kMicroSecondsPerSecond = 1000 * 1000;
	};
    //内联函数减少函数调用的时间消耗
    inline bool operator<(const MicroTimestamp &lhs, const MicroTimestamp &rhs)
    {  
        return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
    }
    
    inline bool operator==(const MicroTimestamp &lhs, const MicroTimestamp &rhs)
    {  
        return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
    }
    
    inline double timeDifference(const MicroTimestamp &high, const MicroTimestamp &low)
    {
        int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
        return static_cast<double>(diff) / MicroTimestamp::kMicroSecondsPerSecond;
    }

    inline MicroTimestamp addTime(const MicroTimestamp &timestamp, const double seconds)
    {
        int64_t delta = static_cast<int64_t>(seconds * MicroTimestamp::kMicroSecondsPerSecond);
        return MicroTimestamp(timestamp.microSecondsSinceEpoch() + delta);
    }

}
#endif
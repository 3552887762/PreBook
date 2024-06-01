#ifndef __LOG_MESSAGE_H__
#define __LOG_MESSAGE_H__
#include<iostream>
#include<sstream>
#include<string>
using namespace std;
#include"LogCommon.hpp"

namespace tulun
{
    class LogMessage
    {
    private:
        std::string Header;   // [时间戳];[日志级别];[文件名称];[函数名称];[行号]
        std::string m_text;  // 正文信息
        LOG_LEVEL   m_level;
    public:
        LOG_LEVEL getLogLevel() const;
    public:
    //              [日志级别];[文件名称];[函数名称];[行号]  
        LogMessage(LOG_LEVEL level,const string & filename,const string & funcname,int linenumber);
        const string toString() const ;
        //  添加正文信息
        template<typename T>
        LogMessage & operator<<(const T &text)
        {
            stringstream ss;
            ss<<" : "<<text;
            m_text+=ss.str();
            return *this;
        }
    };
}

#endif 

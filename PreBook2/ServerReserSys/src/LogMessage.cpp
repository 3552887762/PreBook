

#include "../include/LogCommon.hpp"
#include "../include/LogMessage.hpp"
#include "../include/Timestamp.hpp"
#include <sstream>
using namespace std;
namespace tulun
{

    // std::string Header;   // [时间戳];[日志级别];[文件名称];[函数名称];[行号]
    // std::string m_text;  // 正文信息
    //              [日志级别];[文件名称];[函数名称];[行号]
    LogMessage::LogMessage(LOG_LEVEL level, const string &filename, const string &funcname, int linenumber)
        : m_level(level)
    {
        std::stringstream ss;
        ss << MicroTimestamp::now().toFormattedString() << " ";
        ss << LLtoStr[static_cast<int>(level)] << " ";
        ss << filename << " " << funcname << " " << linenumber << " : ";
        Header = ss.str();
    }
    const string LogMessage::toString() const
    {
        return Header + m_text;
    }

    LOG_LEVEL LogMessage::getLogLevel() const
    {
        return m_level;
    }

}
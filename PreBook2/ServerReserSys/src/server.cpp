//C++ STL
#include<iostream>
using namespace std;
// C API
#include<stdlib.h>
#include"../include/Logger.hpp"
#include"../include/LogCommon.hpp"
#include"../include/MyLibevent.hpp"

int main()
{
    tulun::Logger::setLogLevel(tulun::LOG_LEVEL::TRACE);
    MyLibevent *plib = nullptr;
    try
    {
        plib = new  MyLibevent();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        LOG_FATAL << "MyLibevent Create fail";
        exit(1);
    }
    LOG_TRACE << "MyLibevent Create Success";
    
    if (!plib->MyLibevent_Init())
    {
        LOG_FATAL << "MyLibevent_Init fail ";
        exit(1);
    }
    LOG_TRACE << "MyLibevent Init Success";

    LOG_TRACE << "event Dispatch ...start";
    if (!plib->MYLibevent_Dispatch())
    {
        LOG_ERROR << " MyLibevent_Dispatch fail ";
        exit(1);
    }
    LOG_TRACE << "event Dispatch ... end ";

    delete plib;
    plib = nullptr;
    LOG_TRACE << " Server quit ";
    return 0;
}
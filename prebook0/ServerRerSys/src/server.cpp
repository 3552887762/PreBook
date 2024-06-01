#include "../include/EventObj.hpp"
#include "../include/MyLibevent.hpp"
#include "../include/sqlconn.hpp"
#include"../include/Logger.hpp"
#include"../include/LogCommon.hpp"
#include <cstring>
#include<iostream>
#include <stdlib.h>
using namespace std;

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
    Socket sock;
    if (!sock.Socket_init())
    {
        cout << "sockfd err" << endl;
        exit(1);
    }


    Accept_Obj *pObj = new Accept_Obj(sock.Get_Sockfd(), plib);
    if (pObj == NULL)
    {
        exit(1);
    }

    plib->MyLibevent_Add(sock.Get_Sockfd(), pObj);

    if(!plib->MyLibevent_Dispatch())
    {      
        LOG_ERROR << " MyLibevent_Dispatch fail ";
        exit(1);
    }
    LOG_TRACE << "event Dispatch ... end ";
    

    delete pObj;
    delete plib;
    plib = nullptr;
    LOG_TRACE << " Server quit ";
    exit(0);
}
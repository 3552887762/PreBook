// C++ string class
#include <string>  
using namespace std;
// C API
#include<string.h>  
// Liunx API
#include<arpa/inet.h>
#include<event.h>
#ifndef MYLIBEVENT_HPP
#define MYLIBEVENT_HPP
#define LIST_MAX 128


class MyLibevent
{
private:
    struct event_base *m_base;
    Event_Accept *pSock;
    int sockfd;
    short m_port;
    std::string m_ips;

private:
    bool Socket_Init();

public:
    MyLibevent();

    MyLibevent(const string &ips, const short port);

    bool MyLibevent_Init();

    bool MyLibevent_Add(int fd, Event_Obj *pObj);
    bool MYLibevent_Dispatch();

    ~MyLibevent();
};
#endif 
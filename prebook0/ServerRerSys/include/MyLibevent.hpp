
#ifndef MYLIBEVENT_HPP
#define MYLIBEVENT_HPP

#include<arpa/inet.h>
#include<event.h>


class Sock_Obj;

class MyLibevent
{
public:
    MyLibevent()
    {
        m_base = NULL;
    }
    bool MyLibevent_Init()
    {
        m_base = event_init();
        if (m_base == NULL)
        {
            return false;
        }

        return true;
    }

    bool MyLibevent_Add(int fd, Sock_Obj *pObj); // sockfd监听套接字 accept ->obj, c 连接套接字 recv
    void MyLibevent_Delete(Sock_Obj *pObj);
    bool MyLibevent_Dispatch();


private:
    struct event_base *m_base;
};

#endif
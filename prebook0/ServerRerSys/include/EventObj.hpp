
#ifndef EVENTOBJ_HPP
#define EVENTOBJ_HPP
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <event.h>
#include <jsoncpp/json/json.h>
using namespace std;

class MyLibevent;

enum OP_TYPE
{
    DL = 1,
    ZC,
    CKYY,
    YD,
    YDXX,
    QXYD,
    TC
};
class Socket
{
public:
    Socket()
    {
        m_ips = "127.0.0.1";
        m_port = 6000;
        m_sockfd = -1;
    }

    Socket(const string &ips, short port) : m_ips(ips), m_port(port)
    {
        m_sockfd = -1;
    }

    bool Socket_init();

    int Get_Sockfd() const
    {
        return m_sockfd;
    }

    ~Socket()
    {
        close(m_sockfd);
    }

private:
    string m_ips;
    short m_port;
    int m_sockfd;
};

//
class Event_Obj
{
public:
    virtual void CallBack_Fun() = 0;
    struct event *ev;
    MyLibevent *plib;
};


// 处理监听套接字
class Event_Accept : public Event_Obj
{
public:
    Event_Accept(int fd, MyLibevent *p)
    {
        sockfd = fd;
        plib = p;
    }
    virtual void CallBack_Fun();

private:
    int sockfd;
};

// 处理连接套接字
class Event_Recv : public Event_Obj
{
public:
    Event_Recv(int fd, MyLibevent *p) : c(fd)
    {
        plib = p;
    }
    virtual void CallBack_Fun();
    ~Event_Recv()
    {
        close(c);
        cout << "client close" << endl;
    }

private:
    void Send_OK();
    void Send_ERR();
    void Send_Json(Json::Value &val) const;

    void User_zc();
    void User_dl();

    void Show_YuYue();
    void User_yd();
    void Show_user_yd();
    void Delete_user_yd();
private:
    int c;
    Json::Value m_val;
};

#endif
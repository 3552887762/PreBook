#include "../include/EventObj.hpp"
#include "../include/MyLibevent.hpp"
#include "../include/Logger.hpp"
#include "../include/LogCommon.hpp"

static void Event_Call_Back(int fd, short ev, void *arg)
{
    LOG_TRACE << "in Event_Call_Back() ...";
    Event_Obj *pObj = (Event_Obj *)arg;
    if (pObj == NULL)
    {
        LOG_DEBUG << "Event_Obj NULL pointer ";
        return;
    }

    if (ev & EV_READ)
    {
        LOG_TRACE << "EV_READ Event  到达: " << ev;
        LOG_TRACE << "call pObj->CallBack_Fun";
        LOG_TRACE << "pObj => Object Type :" << typeid(*pObj).name();
        pObj->CallBack_Fun();
    }
    LOG_TRACE << "out Event_Call_Back() ...";
}

// struct event_base *m_base;
// Event_Accept *pSock;
// int sockfd;
// short m_port;
// std::string m_ips;

bool MyLibevent::Socket_Init()
{
    LOG_TRACE << "IN Socket_Init()";
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        LOG_DEBUG << "sockfd == -1";
        return false;
    }
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(m_port);
    saddr.sin_addr.s_addr = inet_addr(m_ips.c_str());
    int res = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (-1 == res)
    {
        LOG_DEBUG << "bind fail";
        return false;
    }
    res = listen(sockfd, LIST_MAX);
    if (-1 == res)
    {
        LOG_DEBUG << "listen fail";
        return false;
    }
    LOG_TRACE << "OUT Socket_Init()";
    return true;
}

MyLibevent::MyLibevent()
    : m_base(nullptr),
      pSock(nullptr),
      sockfd(-1),
      m_ips("127.0.0.1"),
      m_port(6000)
{
    LOG_TRACE << "Create MyLibevent Object ";
}
MyLibevent::MyLibevent(const string &ips, const short port)
    : m_ips(ips),
      m_port(port),
      m_base(nullptr),
      pSock(nullptr),
      sockfd(-1)
{
    LOG_TRACE << "Create MyLibevent() Object";
}

bool MyLibevent::MyLibevent_Init()
{
    LOG_TRACE << "in MyLibevent_Init ...";
    m_base = event_init();
    if (nullptr == m_base)
    {
        LOG_DEBUG << "libevent init error ";
        return false;
    }
    if (!Socket_Init())
    {
        LOG_DEBUG << "socket init error ";
        return false;
    }
    try
    {
        pSock = new Event_Accept(sockfd, this);
    }
    catch (const std::exception &e)
    {
        LOG_FATAL << e.what();
        return false;
    }
    if (!MyLibevent_Add(sockfd, pSock))
    {
        LOG_TRACE << "MyLibevent_Add fail";
        return false;
    }
    LOG_TRACE << "out MyLibevent_Init";
    
    return true;
}
bool MyLibevent::MyLibevent_Add(int fd, Event_Obj *pObj)
{
    LOG_TRACE << "In MyLibevetn_Add ";
    if (-1 == fd || nullptr == pObj)
    {
        LOG_DEBUG << " fd == -1 || pObj == nullptr)";
        return false;
    }
    LOG_TRACE << "event_new set Event_Call_Back";
    struct event *ev = event_new(m_base, fd, EV_READ | EV_PERSIST, Event_Call_Back, pObj);
    
    if (nullptr == ev)
    {
        LOG_DEBUG << "event_new fail";
        return false;
    }
    pObj->ev = ev;
    if (event_add(ev, nullptr) == -1)
    {
        LOG_DEBUG << "event_add(ev,nullptr) fail";
        event_free(ev);
        return false;
    }
    LOG_TRACE << "OUT MyLibevetn_Add";
   
    return true;
}
bool MyLibevent::MYLibevent_Dispatch()
{
    if (event_base_dispatch(m_base) == -1)
    {
        clog << "event base dispach error " << endl;
        return false;
    }
    return true;
}

MyLibevent::~MyLibevent()
{
    close(sockfd);
    delete pSock;
    pSock = nullptr;
    event_base_free(m_base);
    m_base = nullptr;
}

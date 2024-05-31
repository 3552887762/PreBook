

static void Event_Call_Back(int fd, short ev, void *arg)
{
    Event_Obj *pObj = (Event_Obj *)arg;
    if (pObj == NULL)
    {
        return;
    }

    if (ev & EV_READ)
    {
        pObj->CallBack_Fun();
    }
}

// struct event_base *m_base;
// Event_Accept *pSock;
// int sockfd;
// short m_port;
// std::string m_ips;

bool MyLibevent::Socket_Init()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
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
        return false;
    }
    res = listen(sockfd, LIST_MAX);
    if (-1 == res)
    {
        return false;
    }
    return true;
}

MyLibevent::MyLibevent()
    : m_base(nullptr),
      pSock(nullptr),
      sockfd(-1),
      m_ips("127.0.0.1"),
      m_port(6000)
{
}
MyLibevent::MyLibevent(const string &ips, const short port)
    : m_ips(ips),
      m_port(port),
      m_base(nullptr),
      pSock(nullptr),
      sockfd(-1)
{
}

bool MyLibevent::MyLibevent_Init()
{
    m_base = event_init();
    if (nullptr == m_base)
    {
        clog << "libevent init error " << endl;
        return false;
    }
    if (!Socket_Init())
    {
        clog << "socket init error " << endl;
        return false;
    }
    try
    {
        pSock = new Event_Accept(sockfd, this);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    if (!MyLibevent_Add(sockfd, pSock))
    {
        return false;
    }
    return true;
}
bool MyLibevent::MyLibevent_Add(int fd, Event_Obj *pObj)
{
    if (-1 == fd || nullptr == pObj)
    {
        return false;
    }
    struct event *ev = event_new(m_base, fd, EV_READ | EV_PERSIST, Event_Call_Back, pObj);
    if (nullptr == ev)
    {
        return false;
    }
    pObj->ev = ev;
    if (event_add(ev, nullptr) == -1)
    {
        event_free(ev);
        return false;
    }
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

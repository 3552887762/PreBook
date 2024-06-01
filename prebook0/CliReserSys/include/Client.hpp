#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <jsoncpp/json/json.h>

using namespace std;
class Client
{
public:
    Client()
    {
        ips = "127.0.0.1";
        port = 6000;
        sockfd = -1;
        runing = true;
        dl_status = false;
        User_Op = -1;
    }
    Client(const string & ip, short port)
    {
        ips = ip;
        this->port = port;
        sockfd = -1;
        runing = true;
        dl_status = false;
        User_Op = -1;
    }

    ~Client()
    {
        close(sockfd);
    }
    bool Connect();

    void Run();

private:
    void Show_Menu();
    void User_zc();
    void User_dl();
    void Show_YuYue();
    void User_yd();
    void Show_user_yd();
    void Delete_user_yd();

    void Send_Json(const Json::Value &val);

private:
    int sockfd;
    string ips;
    short  port;

    bool runing;
    bool dl_status;

    int  User_Op;
    string user_name;
    string user_tel;

    map<int,string> m_map;
    map<int,string> m_map_yd;

};
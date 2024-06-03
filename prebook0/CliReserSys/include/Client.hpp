#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <jsoncpp/json/json.h>

using namespace std;
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
    bool ConnectToServer();

    void Run();

private:
    void Show_Menu();
    void User_Register();
    void User_Login();
    void  User_Check_PreBook();
    void User_Predet();
    void User_Check_Predet();
    void User_Cancel_Predet();
    string recv_data();
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

#endif
#ifndef CLIENT_HPP
#define CLIENT_HPP
// C++
#include<iostream>
#include<string>
#include<map>
using namespace std;
// C API
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// Liunx API
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
// CJSON
#include <jsoncpp/json/json.h>

typedef enum
{
    LOGIN = 1,      // 登录
    REGISTER,       // 注册
    CHECK_PRE_BOOK, // 查看预约
    PREDET,         // 预定
    CHECK_PREDET,   // 查看预定
    CANCEL_PREDET,  // 取消预定
    QUIT            // 退出
} CHO_TYPE;

class Client
{
private:
    int sockfd;

    string m_ips;
    short m_port;

    bool runing;
    bool status_dl;

    string user_name;
    string user_tel;

    int user_op;
    map<int, string> m_map;
    map<int, string> yd_map;

public:
    Client()
    {
        sockfd = -1;
        m_ips = "127.0.0.1";
        m_port = 6000;
        runing = true;
        status_dl = false;
    }

    Client(const string &ips, short port) : m_ips(ips), m_port(port)
    {
        sockfd = -1;
        runing = true;
        status_dl = false;
    }

    ~Client()
    {
        close(sockfd);
    }

    bool ConnectToServer();
    void Run();

private:
    void print_info();
    void Send_Json(Json::Value &val);

    void User_Login();         // 登录
    void User_Register();      // 注册
    void User_Check_PreBook(); // 查看预约
    void User_Predet();        // 预定
    void User_Check_Predet();  // 查看预定
    void User_Cancel_Predet(); // 取消预定
    void User_Quit();          // 退出
/************
    void User_dl();
    void User_zc();

    void User_ckyy();
    void User_yd();

    void User_ckyd();
    void User_qxyd();
*/
};
#endif
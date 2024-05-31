//C++
#include <iostream>
using namespace std;
//Liunx 
#include <sys/socket.h>
#include <unistd.h>

#include "jsoncpp/json/json.h"
#include <event.h>

#ifndef EVENTOBJ_HPP
#define EVENTOBJ_HPP

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

class Event_Obj
{
public:
    virtual void CallBack_Fun() = 0;
    struct event *ev = nullptr;
};

class Event_Accept : public Event_Obj
{
public:
    Event_Accept(int fd, MyLibevent *p);
    virtual void CallBack_Fun();

private:
    int sockfd;
    MyLibevent *plib;
};

class Event_Recv : public Event_Obj
{
public:
    virtual void CallBack_Fun();
    Event_Recv(int fd);
    ~Event_Recv();

private:
    void User_Login();         // 登录
    void User_Register();      // 注册
    void User_Check_PreBook(); // 查看预约
    void User_Predet();        // 预定
    void User_Check_Predet();  // 查看预定
    void User_Cancel_Predet(); // 取消预定
    void User_Quit();          // 退出

    /*
    void User_zc();
    void User_dl();
    void User_ckyy();
    void User_yd();
    void User_ckyd();
    void User_qxyd();
    */

    void Send_Ok();
    void Send_Err();
    void Send_Json(Json::Value &rval) const;

private:
    int c;
    Json::Value val;
};

#endif
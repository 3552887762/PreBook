
#include "../include/sqlconn.hpp"
#include "../include/MyLibevent.hpp"
#include "../include/EventObj.hpp"
#include "../include/Logger.hpp"
// Event_Accept;
Event_Accept::Event_Accept(int fd, MyLibevent *p)
    : sockfd(fd),
      plib(p)
{
    LOG_TRACE << "Create Event_Accept Object";
}
void Event_Accept::CallBack_Fun()
{
    LOG_TRACE << "IN Event_Accept:: CallBack_Fun";
    int c = accept(sockfd, NULL, NULL);
    if (c < 0)
    {
        LOG_DEBUG << "accept fail";
        return;
    }
    Event_Recv *r = nullptr;
    LOG_TRACE << "new Event_Recv";
    try
    {
        r = new Event_Recv(c);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        LOG_DEBUG << "event_recv create error ";
        close(c);
        return;
    }
    LOG_TRACE << "MyLibevent_Add(): " << c;
    if (!plib->MyLibevent_Add(c, r))
    {
        delete r;
        LOG_DEBUG << "MyLibevent_Add fail";
        return;
    }
    LOG_TRACE << "OUT Event_Accept:: CallBack_Fun";
}

// Event_Recv;
void Event_Recv::Send_Ok()
{
    LOG_TRACE << " IN Event_Recv::Send_Ok";
    Json::Value resval;
    resval["status"] = "OK";
    send(c, resval.toStyledString().c_str(), resval.toStyledString().size(), 0);
    LOG_TRACE << " OUT Event_Recv::Send_Ok";
}
void Event_Recv::Send_Err()
{
    LOG_TRACE << " IN Event_Recv::Send_Err";
    Json::Value resval;
    resval["status"] = "ERR";
    send(c, resval.toStyledString().c_str(), resval.toStyledString().size(), 0);
    LOG_TRACE << "OUT Event_Recv::Send_Err";
}
void Event_Recv::Send_Json(Json::Value &rval) const
{
    LOG_TRACE << "IN Event_Recv::Send_Json";
    send(c, rval.toStyledString().c_str(), rval.toStyledString().size(), 0);
    LOG_TRACE << " OUT Event_Recv::Send_Json";
}
Event_Recv::Event_Recv(int fd)
    : c(fd)
{
    LOG_TRACE << "Create Event_Recv" << fd;
}
Event_Recv::~Event_Recv()
{
    close(c);
    LOG_TRACE << "client close";
}
void Event_Recv::CallBack_Fun()
{
    LOG_TRACE << "IN Event_Recv::CallBack_Funn";
    char buff[512] = {0};
    int n = recv(c, buff, 512, 0); // 收到json格式字符串
    if (n <= 0)
    {
        LOG_DEBUG << "recv ..fail " << n;
        event_free(ev); // ev 在基类中
        LOG_TRACE << "Destroy Event_Recv Object";
        delete this;
        return;
    }

    LOG_TRACE << "buff: " << buff;
    Json::Reader Read;
    if (!Read.parse(buff, val)) // 对json格式字符串反序列化，生成json对象
    {
        LOG_DEBUG << "json err";
        return;
    }

    int user_op = val["type"].asInt(); // 根据key,获取值
    // DL=1,ZC,CKYY,YD,CKYD,QXYD
    LOG_TRACE << "user_op : " << cho_type[user_op];
    switch (user_op)
    {
    case LOGIN:
        User_Login();
        break;
    case REGISTER:
        User_Register();
        break;
    case CHECK_PRE_BOOK:
        User_Check_PreBook();
        break;
    case PREDET:
        User_Predet();
        break;
    case CHECK_PREDET:
        User_Check_Predet();
        break;
    case CANCEL_PREDET:
        User_Cancel_Predet();
        break;
    case QUIT:
        User_Quit();
        break;

    default:
        break;
    }
}
// 注册
void Event_Recv::User_Register()
{
    LOG_TRACE << "IN Event_Recv::User_Register";
    string user_tel = val["user_tel"].asString();
    string user_name = val["user_name"].asString();
    string user_passwd = val["user_passwd"].asString();
    LOG_TRACE << "Tel " << user_tel;
    LOG_TRACE << "name: " << user_name;
    LOG_TRACE << "passwd: " << user_passwd;
    Mysql_Client cli;
    LOG_TRACE << "定义: Object MySql_Client";
   
    if (!cli.ConnectToMysql())
    {
        LOG_DEBUG << "Connect MySql Failed";

        Send_Err();
        return;
    }
    LOG_TRACE << "Register user";
    if (!cli.Mysql_TelRegister(user_tel, user_name, user_passwd))
    {
        LOG_DEBUG << "Register failed";
        Send_Err();
        return;
    }
    
    Send_Ok();
    LOG_TRACE << "OUT Event_Recv::User_Register";
    return;
}
// 登录
void Event_Recv::User_Login()
{
    LOG_TRACE << "In Event_Recv::User_Login";
    string user_tel = val["user_tel"].asString();
    string user_passwd = val["user_passwd"].asString();
    LOG_TRACE << "tel: " << user_tel;
    LOG_TRACE << "passwd: " << user_passwd;
    if (user_tel.empty() || user_passwd.empty())
    {
        LOG_DEBUG << "user_tel.empty || user_passwd.empty";
        Send_Err();
        return;
    }
    LOG_TRACE << "Create MySql_Client Object";
    Mysql_Client cli;
    if (!cli.ConnectToMysql())
    {
        LOG_DEBUG << "Connect mySql fail";
        Send_Err();
        return;
    }
    LOG_TRACE << "cli.MySql_Login:" << user_tel << " " << user_passwd;
    LogInNode node = cli.Mysql_Login(user_tel, user_passwd);
    LOG_TRACE << "node" << node.tel << " " << node.name << " " << node.pass << " " << node.tag;
    if (!node.tag)
    {
        LOG_DEBUG << "Login fail";
        Send_Err();
        return;
    }
    Json::Value resval;
    resval["status"] = "OK";
    resval["user_name"] = node.name;
    Send_Json(resval);
    LOG_TRACE << "OUT Event_Recv::User_Login";
}
// 查看预约
void Event_Recv::User_Check_PreBook()
{
    Mysql_Client cli;
    if (!cli.ConnectToMysql())
    {
        Send_Err();
        return;
    }

    Json::Value resval;
    if (!cli.Mysql_GetRegisterInfo(resval))
    {
        Send_Err();
        return;
    }

    Send_Json(resval);
}
// 预定
void Event_Recv::User_Predet()
{
    string tel = val["user_tel"].asString();
    string tk_id = val["tk_id"].asString();

    Mysql_Client cli;
    if (!cli.ConnectToMysql())
    {
        Send_Err();
        return;
    }

    if (!cli.Mysql_Schedule(tel, tk_id))
    {
        Send_Err();
        return;
    }

    Send_Ok();
    return;
}
// 查看预定
void Event_Recv::User_Check_Predet()
{
    string tel = val["user_tel"].asString();
    // 操作数据库

    Mysql_Client cli;
    if (!cli.ConnectToMysql())
    {
        Send_Err();
        return;
    }

    Json::Value resval;
    if (!cli.MySql_Get_TelRegInfo(tel, resval))
    {
        Send_Err();
        return;
    }

    Send_Json(resval);
}
// 取消预定
void Event_Recv::User_Cancel_Predet()
{
    string yd_id = val["yd_id"].asString();
    Mysql_Client cli;
    if (!cli.ConnectToMysql())
    {
        Send_Err();
        return;
    }

    if (!cli.Mysql_CancelTelRegister(yd_id))
    {
        Send_Err();
        return;
    }

    Send_Ok();
    return;
}

// 退出
void Event_Recv::User_Quit()
{
    LOG_TRACE << "Event_Recv::User_Quit";
}

/*
void User_zc();
void User_dl();
void User_ckyy();
void User_yd();
void User_ckyd();
void User_qxyd();
*/

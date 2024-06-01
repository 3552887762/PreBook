#include "../include/sqlconn.hpp"
#include "../include/MyLibevent.hpp"
#include "../include/EventObj.hpp"
#include "../include/Logger.hpp"
#include <cstring>

#define LIST_MAX 128

bool Socket::Socket_init()
{
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd == -1)
    {
        return false;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(m_port);
    saddr.sin_addr.s_addr = inet_addr(m_ips.c_str());

    if (bind(m_sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
    {
        return false;
    }

    if (listen(m_sockfd, LIST_MAX) == -1)
    {
        return false;
    }

    return true;
}

void Accept_Obj::CallBack_Fun()
{
    int c = accept(sockfd, NULL, NULL);
    if (c < 0)
    {
        return;
    }

    Recv_Obj *precv = new Recv_Obj(c, plib);
    if (precv == NULL)
    {
        close(c);
        return;
    }

    plib->MyLibevent_Add(c, precv);
    cout << "accept client:" << c << endl;
}
void Recv_Obj::Send_OK()
{
    Json::Value val;
    val["status"] = "OK";
    send(c, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
}
void Recv_Obj::Send_ERR()
{
    Json::Value val;
    val["status"] = "ERR";
    send(c, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
}
void Recv_Obj::Send_Json(Json::Value &val)
{
    send(c, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
}
void Recv_Obj::User_zc()
{
    string user_tel = m_val["user_tel"].asString();
    string user_name = m_val["user_name"].asString();
    string user_password = m_val["user_password"].asString();

    MysqlClient mysqlcli;
    if (!mysqlcli.Connect_toDb())
    {
        Send_ERR();
        return;
    }

    if (!mysqlcli.Dd_user_zc(user_name, user_tel, user_password))
    {
        Send_ERR();
        return;
    }

    Send_OK();
    return;
}
void Recv_Obj::User_dl()
{
    string user_tel = m_val["user_tel"].asString();
    string user_password = m_val["user_password"].asString();

    string user_name;

    MysqlClient mysqlcli;
    if (!mysqlcli.Connect_toDb())
    {
        Send_ERR();
        return;
    }

    if (!mysqlcli.Dd_user_dl(user_name, user_tel, user_password))
    {
        Send_ERR();
        return;
    }

    Json::Value val;
    val["status"] = "OK";
    val["user_name"] = user_name;
    Send_Json(val);
}

void Recv_Obj::Show_YuYue()
{
    Json::Value resval;
    MysqlClient cli;
    if (!cli.Connect_toDb())
    {
        Send_ERR();
        return;
    }

    if (!cli.Db_show_yuyue(resval))
    {
        Send_ERR();
        return;
    }

    resval["status"] = "OK";
    Send_Json(resval);
    return;
}

void Recv_Obj::User_yd()
{
    string user_tel = m_val["user_tel"].asString();
    string tk_id = m_val["tk_id"].asString();

    MysqlClient cli;
    if (!cli.Connect_toDb())
    {
        Send_ERR();
        return;
    }

    if (!cli.Db_user_yd(user_tel, tk_id))
    {
        Send_ERR();
        return;
    }

    Send_OK();
    return;
}
void Recv_Obj::Show_user_yd()
{
    Json::Value resval;
    string user_tel = m_val["user_tel"].asString();

    MysqlClient cli;
    if (!cli.Connect_toDb())
    {
        Send_ERR();
        return;
    }

    if (!cli.Db_show_yd(resval, user_tel))
    {
        Send_ERR();
        return;
    }

    resval["status"] = "OK";
    Send_Json(resval);
    return;
}
void Recv_Obj::Delete_user_yd()
{
    string res_id = m_val["res_id"].asString();

    MysqlClient cli;
    if( !cli.Connect_toDb())
    {
        return;
    }

    if ( !cli.Db_Delete_yd(res_id))
    {
        Send_ERR();
        return;
    }

    Send_OK();

    return;
}
void Recv_Obj::CallBack_Fun()
{
    char buff[256] = {0};
    int n = recv(c, buff, 255, 0); // 接受json数据
    if (n <= 0)
    {
        plib->MyLibevent_Delete(this);
        delete this;
        return;
    }

    cout << "recv:" << buff << endl;
    m_val.clear();

    Json::Reader Read;
    if (!Read.parse(buff, m_val))
    {
        cout << "json 解析失败" << endl;
        Send_ERR();
        return;
    }

    const int User_Op = m_val["type"].asInt();
    switch (User_Op)
    {
    case DL:
        User_dl();
        break;
    case ZC:
        User_zc();
        break;
    case CKYY:
        Show_YuYue();
        break;
    case YD:
        User_yd();
        break;
    case YDXX:
        Show_user_yd();
        break;
    case QXYD:
        Delete_user_yd();
        break;

    default:
        break;
    }

}

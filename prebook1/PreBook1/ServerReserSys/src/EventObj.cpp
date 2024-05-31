

// Event_Accept;
Event_Accept::Event_Accept(int fd, MyLibevent *p)
    : sockfd(fd),
      plib(p)
{
}
void Event_Accept::CallBack_Fun()
{
    int c = accept(sockfd, NULL, NULL);
    if (c < 0)
    {
        return;
    }
    Event_Recv *r = nullptr;
    try
    {
        r = new Event_Recv(c);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        clog << "event_recv create error " << endl;
        close(c);
        return;
    }
    if (!plib->MyLibevent_Add(c, r))
    {
        delete r;
        return;
    }
}

// Event_Recv;
void Event_Recv::Send_Ok()
{
    Json::Value resval;
    resval["status"] = "OK";
    send(c, resval.toStyledString().c_str(), resval.toStyledString().size(), 0);
}
void Event_Recv::Send_Err()
{
    Json::Value resval;
    resval["status"] = "ERR";
    send(c, resval.toStyledString().c_str(), resval.toStyledString().size(), 0);
}
void Event_Recv::Send_Json(Json::Value &rval) const
{
    send(c, rval.toStyledString().c_str(), rval.toStyledString().size(), 0);
}
Event_Recv::Event_Recv(int fd)
    : c(fd)
{
}
Event_Recv::~Event_Recv()
{
    close(c);
    clog << "client close" << endl;
}
void Event_Recv::CallBack_Fun()
{
    char buff[512] = {0};
    int n = recv(c, buff, 512, 0); // 收到json格式字符串
    if (n <= 0)
    {
        event_free(ev); // ev 在基类中
        delete this;
        return;
    }

    cout << buff << endl;

    Json::Reader Read;
    if (!Read.parse(buff, val)) // 对json格式字符串反序列化，生成json对象
    {
        cout << "json err" << endl;
        return;
    }

    int user_op = val["type"].asInt(); // 根据key,获取值
    // DL=1,ZC,CKYY,YD,CKYD,QXYD
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
    string user_tel = val["user_tel"].asString();
    string user_name = val["user_name"].asString();
    string user_passwd = val["user_passwd"].asString();
    Mysql_Client cli;
    if (!cli.ConnectToMysql())
    {
        clog << "Connect MySql Failed" << endl;
        Send_Err();
        return;
    }
    if (!cli.Mysql_TelRegister(user_tel, user_name, user_passwd))
    {
        clog << "Register failed" << endl;
        Send_Err();
        return;
    }
    Send_Ok();
    return;
}
// 登录
void Event_Recv::User_Login()
{
    string user_tel = val["user_tel"].asString();
    string user_passwd = val["user_passwd"].asString();
    if (user_tel.empty() || user_passwd.empty())
    {
        Send_Err();
        return;
    }
    Mysql_Client cli;
    if (!cli.ConnectToMysql())
    {
        Send_Err();
        return;
    }
    LogInNode node = cli.Mysql_Login(user_tel, user_passwd);
    if (!node.tag)
    {
        Send_Err();
        return;
    }
    Json::Value resval;
    resval["status"] = "OK";
    resval["user_name"] = node.name;
    Send_Json(resval);
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
}

/*
void User_zc();
void User_dl();
void User_ckyy();
void User_yd();
void User_ckyd();
void User_qxyd();
*/


#include"../include/Client.hpp"

bool Client::ConnectToServer()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        return false;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(m_port);
    saddr.sin_addr.s_addr = inet_addr(m_ips.c_str());

    int res = connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (res == -1)
    {
        return false;
    }

    return true;
}

void Client::print_info()
{
    if (!status_dl)
    {
        cout << "-------用户：游客-------状态：未登陆-----" << endl;
        cout << "1 登陆   2  注册   3  退出" << endl;
        cin >> user_op;
        if (user_op == 3)
        {
            user_op = QUIT;
        }
    }
    else
    {
        cout << "-------用户：" << user_name << "-------状态：已登录-----" << endl;
        cout << "1 查看预约信息       2  预定" << endl;
        cout << "3 查看我的预约       4  取消" << endl;
        cout << "5 退出" << endl;
        cin >> user_op;
        user_op += 2;
    }
    cout << "请输入选择：" << endl;
}
void Client::Send_Json(Json::Value &val)
{
    send(sockfd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
}
void Client::User_Login()
{
    cout << "输入手机号码" << endl;
    cin >> user_tel;
    cout << "输入密码" << endl;
    string user_passwd;
    cin >> user_passwd;

    Json::Value val;
    val["type"] = LOGIN;
    val["user_tel"] = user_tel;
    val["user_passwd"] = user_passwd;

    Send_Json(val);

    char buff[128] = {0};
    int n = recv(sockfd, buff, 127, 0);
    if (n <= 0)
    {
        cout << "ser close" << endl;
        return;
    }

    val.clear();
    // Json::Value resval;
    Json::Reader Read;
    if (!Read.parse(buff, val))
    {
        cout << "json 解析失败" << endl;
        return;
    }

    string status = val["status"].asString();
    if (status.compare("OK") != 0)
    {
        cout << "登陆失败" << endl;
        return;
    }

    user_name = val["user_name"].asString();
    status_dl = true;
    cout << "登陆成功" << endl;
}
void Client::User_Register()
{
    cout << "输入手机号码:" << endl;
    cin >> user_tel;
    cout << "输入用户名:" << endl;
    cin >> user_name;

    string user_passwd;
    cout << "输入密码:" << endl;
    cin >> user_passwd;

    Json::Value val;
    val["type"] = REGISTER;
    val["user_tel"] = user_tel;
    val["user_name"] = user_name;
    val["user_passwd"] = user_passwd;

    send(sockfd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);

    char buff[128] = {0};
    if (recv(sockfd, buff, 127, 0) <= 0)
    {
        cout << "ser close" << endl;
        return;
    }

    val.clear();
    Json::Reader Read;
    if (!Read.parse(buff, val))
    {
        cout << "json err" << endl;
        return;
    }

    string status_str = val["status"].asString();
    if (status_str.compare("OK") != 0)
    {
        cout << "注册失败" << endl;
        return;
    }

    status_dl = true;
    cout << "注册成功" << endl;
}
void Client::User_Check_PreBook()
{
    Json::Value val;
    val["type"] = CHECK_PRE_BOOK;
    Send_Json(val);

    // 接收服务器的数据
    char buff[1024] = {0};
    int n = recv(sockfd, buff, 1023, 0);
    if (n <= 0)
    {
        cout << "ser close" << endl;
        return;
    }

    Json::Value resval;
    Json::Reader Read;
    if (!Read.parse(buff, resval))
    {
        cout << "jons err" << endl;
        return;
    }

    string status = resval["status"].asString();
    if (status.compare("OK") != 0)
    {
        cout << "查询失败" << endl;
        return;
    }

    int num = resval["num"].asInt();
    if (num == 0)
    {
        cout << "没有可预约的信息" << endl;
        return;
    }

    m_map.clear();
    cout << "序号  地点名称  总票数  预定数  时间" << endl;
    for (int i = 0; i < num; i++)
    {
        m_map.insert(make_pair(i + 1, resval["tk_arr"][i]["tk_id"].asString()));
        cout << i + 1 << "    ";
        cout << resval["tk_arr"][i]["tk_name"].asString() << "  ";
        cout << resval["tk_arr"][i]["tk_max"].asString() << "      ";
        cout << resval["tk_arr"][i]["tk_num"].asString() << "     ";
        cout << resval["tk_arr"][i]["tk_time"].asString() << endl;
    }
}
void Client::User_Predet()
{
    User_Check_PreBook();
    int index = 0;
    cout << "请输入要预约的序号:" << endl;
    cin >> index;

    map<int, string>::iterator pos = m_map.find(index);
    if (pos == m_map.end())
    {
        cout << "输入不合法" << endl;
        return;
    }

    Json::Value val;
    val["type"] = PREDET;
    val["user_tel"] = user_tel;
    val["tk_id"] = pos->second;

    Send_Json(val);

    char buff[128] = {0};
    int n = recv(sockfd, buff, 127, 0);
    Json::Value rval;
    Json::Reader Read;
    if (!Read.parse(buff, rval))
    {
        cout << "json err" << endl;
        return;
    }

    string status = rval["status"].asString();
    if (status.compare("OK") != 0)
    {
        cout << "预定失败" << endl;
        return;
    }

    cout << "预定成功" << endl;

    return;
}
void Client::User_Check_Predet()
{
    Json::Value val;
    val["type"] = CHECK_PREDET;
    val["user_tel"] = user_tel;

    Send_Json(val);

    char buff[1024] = {0};
    int n = recv(sockfd, buff, 1023, 0);
    if (n <= 0)
    {
        cout << "ser close" << endl;
        return;
    }

    Json::Value resval;
    Json::Reader Read;
    if (!Read.parse(buff, resval))
    {
        cout << "json err" << endl;
        return;
    }

    // 解析
    string status = resval["status"].asString();
    if (status.compare("OK") != 0)
    {
        cout << "获取预定信息失败" << endl;
        return;
    }

    int num = resval["num"].asInt();
    if (num == 0)
    {
        cout << "没有预约信息" << endl;
        return;
    }

    cout << "序号 地点名称 时间" << endl;
    yd_map.clear();
    for (int i = 0; i < num; i++)
    {
        yd_map.insert(make_pair(i + 1, resval["tk_arr"][i]["yd_id"].asString()));
        cout << i + 1 << "  ";
        cout << resval["tk_arr"][i]["tk_name"].asString() << " ";
        cout << resval["tk_arr"][i]["tk_time"].asString() << " ";
        cout << endl;
    }
}
void Client::User_Cancel_Predet()
{
    User_Check_Predet();
    cout << "请输入要取消的序号" << endl;
    int index;
    cin >> index;

    map<int, string>::iterator pos = yd_map.find(index);
    if (pos == yd_map.end())
    {
        cout << "输入无效" << endl;
        return;
    }

    Json::Value val;
    val["type"] = CANCEL_PREDET;
    val["yd_id"] = pos->second;
    Send_Json(val);

    char buff[128] = {0};
    int n = recv(sockfd, buff, 127, 0);
    if (n <= 0)
    {
        cout << "ser close" << endl;
        return;
    }

    Json::Value resval;
    Json::Reader Read;
    if (!Read.parse(buff, resval))
    {
        cout << "json err" << endl;
        return;
    }

    string status = resval["status"].asString();
    if (status.compare("OK") != 0)
    {
        cout << "qx err" << endl;
        return;
    }

    cout << "取消成功" << endl;
    return;
}
void Client::Run()
{
    while (runing)
    {
        print_info();
        // DL=1,ZC,CKYY,YD,CKYD,QXYD,TC
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
            runing = false;
            break;

        default:
            cout << "输入不合法，重新输入" << endl;
            break;
        }
    }
}
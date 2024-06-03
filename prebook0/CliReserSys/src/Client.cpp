#include "../include/Client.hpp"
#include <jsoncpp/json/value.h>
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

const string ST_OK = "OK";
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
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ips.c_str());

    if (connect(sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1)
    {
        cout << "Connect err" << endl;
        return false;
    }

    return true;
}

void Client::Run()
{
    while (runing)
    {
        Show_Menu();
        switch (User_Op)
        {
        case DL:
            User_Login();
            break;
        case ZC:
            User_Register();
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
        case TC:
            runing = false;
            break;

        default:
            cout << "无效输入" << endl;
            break;
        }
    }
}

void Client::Show_Menu()
{
    if (!dl_status)
    {
        cout << "---用户名：游客------状态:未登录------" << endl;
        cout << "1  登陆     2  注册     3 退出" << endl;
        cout << "---------------------------------" << endl;
        cout << "请输入选项编号" << endl;
        cin >> User_Op;
        if (User_Op == 3)
        {
            User_Op = TC;
        }
    }
    else
    {
        cout << "---用户名：" << user_name << "------状态：已登录------" << endl;
        cout << "1 查看预约             2  预定 " << endl;
        cout << "3 查看我的预定          4 取消预定" << endl;
        cout << "5 退出" << endl;
        cout << "请输入要执行的序号:" << endl;
        cin >> User_Op;
        User_Op += 2;
    }
}

void Client::Send_Json(const Json::Value &val)
{
    send(sockfd, val.toStyledString().c_str(), strlen(val.toStyledString().c_str()), 0);
}
void Client::User_Register()
{
    cout << "请输入手机号" << endl;
    cin >> user_tel;
    cout << "请输入用户名" << endl;
    cin >> user_name;

    string password1, password2;
    cout << "请输入密码" << endl;
    cin >> password1;
    cout << "请再次输入密码" << endl;
    cin >> password2;

    if (user_tel.empty() || user_name.empty() || password1.empty() || password2.empty())
    {
        cout << "用户信息不能为空" << endl;
        return;
    }

    if (password1.compare(password2) != 0)
    {
        cout << "密码不一致，请重新操作" << endl;
        return;
    }

    Json::Value val;
    val["type"] = ZC;
    val["user_tel"] = user_tel;
    val["user_name"] = user_name;
    val["user_password"] = password1;

    Send_Json(val);

        string s = recv_data();
    
    Json::Value resval;
    Json::Reader Read;
   
    if (!Read.parse(s, resval)){
        cout << "json解析失败" << endl;
        return;
    }

    if (resval["status"].asString().compare(ST_OK) != 0)
    {
        cout << "注册失败" << endl;
        return;
    }
    cout << "注册成功" << endl;
    dl_status = true;
}
void Client::User_Login()
{
    cout << "请输入手机号" << endl;
    cin >> user_tel;
    cout << "请输入密码" << endl;
    string password;
    cin >> password;

    Json::Value val;
    val["type"] = DL;
    val["user_tel"] = user_tel;
    val["user_password"] = password;
    Send_Json(val);

        string s = recv_data();
    
    Json::Value resval;
    Json::Reader Read;
   
    if (!Read.parse(s, resval)){
        cout << "json解析失败" << endl;
        return;
    }
    if (resval["status"].asString().compare(ST_OK) != 0)
    {
        cout << "登陆失败" << endl;
        return;
    }

    cout << "登陆成功" << endl;
    dl_status = true;
    user_name = resval["user_name"].asString();

    return;
}

void Client::Show_YuYue()
{
    Json::Value val;
    val["type"] = CKYY;
    Send_Json(val);

        string s = recv_data();
    
    Json::Value resval;
    Json::Reader Read;
   
    if (!Read.parse(s, resval)){
        cout << "json解析失败" << endl;
        return;
    }

    string status = resval["status"].asString();
    if (status.compare(ST_OK) != 0)
    {
        cout << "查看预约信息失败" << endl;
        return;
    }

    // 打印预约信息
    // cout << buff << endl; // 测试

    int num = resval["num"].asInt();
    if (num == 0)
    {
        cout << "暂无预约信息" << endl;
        return;
    }

    m_map.clear();
    cout << "|序号| 地点名称 | 总票数 | 已预定 | 时间  |" << endl;
    for (int i = 0; i < num; i++)
    {
        // m_map.insert(make_pair(i,resval["arr"]["tk_id"].asString()));
        m_map.insert({i + 1, resval["arr"][i]["tk_id"].asString()});
        cout << i + 1 << "   ";
        cout << resval["arr"][i]["tk_name"].asString() << "   ";
        cout << resval["arr"][i]["tk_max"].asString() << "   ";
        cout << resval["arr"][i]["tk_count"].asString() << "   ";
        cout << resval["arr"][i]["tk_date"].asString() << endl;
    }
}

void Client::User_yd()
{
    Show_YuYue(); // m_map 有了序号和tk_id的对应关系
    cout << "请输入你要预定的序号:" << endl;
    int index = -1;
    cin >> index;

    map<int, string>::iterator pos = m_map.find(index);
    if (pos == m_map.end())
    {
        cout << "输入无效" << endl;
        return;
    }

    string tk_id = pos->second;

    Json::Value val;
    val["type"] = YD;
    val["user_tel"] = user_tel;
    val["tk_id"] = tk_id;

    Send_Json(val);

        string s = recv_data();
    
    Json::Value resval;
    Json::Reader Read;
   
    if (!Read.parse(s, resval)){
        cout << "json解析失败" << endl;
        return;
    }

    string status = resval["status"].asString();
    if (status.compare(ST_OK) != 0)
    {
        cout << "预定失败" << endl;
        return;
    }

    cout << "预定成功" << endl;
    return;
}

string Client::recv_data()
{
    const int SIZE = 1024;
    string s;
    int total_bytes_received = 0;
    int bytes_received = 0;
    do 
    {
        char buff[SIZE] = {0};
        bytes_received = recv(sockfd, buff, SIZE, 0);
        if (bytes_received <= 0)
        {
            cout<<"Error or connection closed by peer"<<endl;
            break;
        }
        s.append(buff, bytes_received);
        total_bytes_received += bytes_received;
    }
    while (bytes_received == SIZE);
    return s;
}

void Client::Show_user_yd()
{
    Json::Value val;
    val["type"] = YDXX;
    val["user_tel"] = user_tel;
    Send_Json(val);
    string s = recv_data();
    
    Json::Value resval;
    Json::Reader Read;
   
    if (!Read.parse(s, resval)){
        cout << "json解析失败" << endl;
        return;
    }
    string status = resval["status"].asString();
    if (status.compare(ST_OK) != 0)
    {
        cout << "查看我的预定信息失败" << endl;
        return;
    }

    int num = resval["num"].asInt();
    if (num == 0)
    {
        cout << "暂无预定信息" << endl;
        return;
    }

    m_map_yd.clear();
    cout << "|序号| 地点名称 |  时间  |" << endl;
    for (int i = 0; i < num; i++)
    {
        // m_map.insert(make_pair(i,resval["arr"]["tk_id"].asString()));
        m_map_yd.insert({i + 1, resval["arr"][i]["res_id"].asString()});
        cout << i + 1 << "   ";
        cout << resval["arr"][i]["tk_name"].asString() << "   ";
        cout << resval["arr"][i]["yd_time"].asString() << endl;
    }
}

void Client::Delete_user_yd()
{
    Show_user_yd();
    cout<<"请输入要取消的序号："<<endl;
    int index = 0;
    cin>>index;

    map<int,string>::iterator pos = m_map_yd.find(index);
    if ( pos == m_map_yd.end())
    {
        cout<<"输入无效"<<endl;
        return;
    }

    string res_id = pos->second;

    Json::Value val;
    val["type"] = QXYD;
    val["res_id"] = res_id;

    Send_Json(val);

    string s = recv_data();
    
    Json::Value resval;
    Json::Reader Read;
   
    if (!Read.parse(s, resval)){
        cout << "json解析失败" << endl;
        return;
    }
    string status_str = resval["status"].asString();
    if ( status_str.compare(ST_OK) != 0 )
    {
        cout<<"取消失败"<<endl;
        return;
    }

    cout<<"取消成功"<<endl;
    return ;
}

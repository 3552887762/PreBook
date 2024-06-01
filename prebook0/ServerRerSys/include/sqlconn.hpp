#include <string>
using namespace std;

#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>

#ifndef SQLCONN_HPP
#define SQLCONN_HPP

class MysqlClient
{

public:
    MysqlClient()
    {
        ips = "127.0.0.1";
        mysql_username = "root";
        mysql_userpasswd = "dl1357135";
        mysql_dbname = "my_database";
        port = 3306;
    }
    bool Connect_toDb();
    bool Dd_user_zc(string name, string tel,string pw);
    bool Dd_user_dl(string &name, string tel,string pw);
    bool Db_show_yuyue(Json::Value &resval);
    bool Db_user_yd(string tel,string tk_id);
    bool Db_show_yd(Json::Value &resval,string tel);
    bool Db_Delete_yd(string res_id);

    void Begin();
    void RollBack();
    void Commit();
    ~MysqlClient()
    {
        mysql_close(&mysql_con);
    }

private:
    MYSQL mysql_con;
    string ips;
    string mysql_username;
    string mysql_userpasswd;
    string mysql_dbname;
    short port;
};
#endif
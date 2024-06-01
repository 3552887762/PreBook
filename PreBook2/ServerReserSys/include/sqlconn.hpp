#include <string>
using namespace std;

#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>

#ifndef SQLCONN_HPP
#define SQLCONN_HPP


typedef struct
{
    std::string tel;
    std::string name;
    std::string pass;
    bool tag; // true; false;
} LogInNode;

class Mysql_Client
{
private:
    string sql_ips;
    int sql_port;
    string sql_name;
    string db_name;
    string db_passwd;
    MYSQL mysql_con;

public:
    Mysql_Client();
    Mysql_Client(const string &ips, const int port, const string &name,
                 const string &dbname, const string &dbpasswd);

    ~Mysql_Client();
    bool ConnectToMysql();
    bool Mysql_TelRegister(const string &tel, const string &name, const string &passwd);
    // bool Mysql_Zc(string tel, string name, string passwd);
    LogInNode Mysql_Login(const string &tel, const string &passwd);
    // bool Mysql_Dl(string tel, string &name,string passwd);
    bool Mysql_GetRegisterInfo(Json::Value &resval);
    // bool Mysql_ckyy(Json::Value &resval);
    // 预定
    bool Mysql_Schedule(const string &tel, const string &ticket_id);
    // bool Mysql_Yd(string tel, string tk_id);
    bool MySql_Get_TelRegInfo(const string &tel, Json::Value &resval);
    // bool Mysql_ckyd(string tel, Json::Value &resval);
    bool Mysql_CancelTelRegister(const string &ticket_id);
    // bool Mysql_qxyd(string yd_id);
};
#endif 
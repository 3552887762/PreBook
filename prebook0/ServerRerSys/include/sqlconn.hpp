#include <string>
using namespace std;

#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>

#ifndef SQLCONN_HPP
#define SQLCONN_HPP

typedef struct
{
    std::string tel;
    std::string name;
    std::string pass;
    bool tag; // true; false;
} LogInNode;

class MysqlClient {

public:
  MysqlClient() {
    ips = "127.0.0.1";
    mysql_username = "root";
    mysql_userpasswd = "dl1357135";
    mysql_dbname = "my_database";
    port = 3306;
  }
  bool ConnectToMysql();
  bool Mysql_TelRegister(const string &tel, const string &name,
                         const string &passwd);
  LogInNode Mysql_Login(const string &tel, const string &passwd);
  bool Mysql_GetRegisterInfo(Json::Value &resval);
  bool Mysql_Schedule(const string &tel, const string &ticket_id);
  bool MySql_Get_TelRegInfo(const string &tel, Json::Value &resval);
  bool Mysql_CancelTelRegister(const string &ticket_id);

  void Begin();
  void RollBack();
  void Commit();
  ~MysqlClient() { mysql_close(&mysql_con); }

private:
  MYSQL mysql_con;
  string ips;
  string mysql_username;
  string mysql_userpasswd;
  string mysql_dbname;
  short port;
};
#endif
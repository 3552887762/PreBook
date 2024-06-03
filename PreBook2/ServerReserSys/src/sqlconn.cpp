#include <iostream>
using namespace std;

#include "../include/Logger.hpp"
#include "../include/sqlconn.hpp" // cmake

// string sql_ips;
// int sql_port;
// string sql_name;
// string db_name;
// string db_passwd;
// MYSQL mysql_con;

Mysql_Client::Mysql_Client()
    : sql_ips("127.0.0.1"), sql_port(3306), sql_name("root"),
      db_name("c2206db"), db_passwd("dl1357135") {}
Mysql_Client::Mysql_Client(const string &ips, const int port,
                           const string &name, const string &dbname,
                           const string &dbpasswd)
    : sql_ips(ips), sql_port(port), sql_name(name), db_name(dbname),
      db_passwd(dbpasswd) {}

Mysql_Client::~Mysql_Client() { mysql_close(&mysql_con); }
bool Mysql_Client::ConnectToMysql() {
  if (mysql_init(&mysql_con) == nullptr) {
    return false;
  }
  if (mysql_real_connect(&mysql_con, sql_ips.c_str(), sql_name.c_str(),
                         db_passwd.c_str(), db_name.c_str(), sql_port, NULL,
                         0) == nullptr) {
    return false;
  }
  return true;
}
bool Mysql_Client::Mysql_TelRegister(const string &tel, const string &name,
                                     const string &passwd) {
  // insert into user_info
  // values(0,'13700000001','小白','123456','1',CurDate());
  string sql = string("insert into user_info values(0,'") + tel +
               string("','") + name + string("','") + passwd +
               string("','1',CurDate())");
  // 测试
  LOG_TRACE << sql;

  if (mysql_query(&mysql_con, sql.c_str()) != 0) {
    return false;
  }
  return true;
}
LogInNode Mysql_Client::Mysql_Login(const string &tel, const string &passwd) {
  LOG_TRACE << " In MySql_Login";
  LogInNode ret = {"", "", "", 0};
  LOG_TRACE << "init ret fail";
  ret.tel = tel;
  ret.pass = passwd;
  ret.tag = false;
  LOG_TRACE << "ret: " << ret.tel << " " << ret.pass << " " << ret.tag;
  // select Name, Passwd from user_info where Tel=13900000002;
  string sql = string("select Name, Passwd from user_info where Tel=") + tel;
  LOG_TRACE << sql;

  if (mysql_query(&mysql_con, sql.c_str()) != 0) {
    return ret;
  }

  MYSQL_RES *res = mysql_store_result(&mysql_con);
  if (res == nullptr) {
    return ret;
  }

  int rownum = mysql_num_rows(res);
  if (0 == rownum) // 没有查找到该用户
  {
    mysql_free_result(res);
    return ret;
  }

  MYSQL_ROW row = mysql_fetch_row(res); // row[0] name, row[1] passwd
  if (passwd.compare(row[1]) != 0) {
    mysql_free_result(res);
    return ret;
  }
  ret.name = row[0];
  ret.tag = true;
  // name = row[0]; // 用户名

  mysql_free_result(res);
  return ret;
}
bool Mysql_Client::Mysql_GetRegisterInfo(Json::Value &resval) {
  string sql = "select * from ticket_table";
  if (mysql_query(&mysql_con, sql.c_str()) != 0) {
    return false;
  }

  MYSQL_RES *res = mysql_store_result(&mysql_con);
  if (res == nullptr) {
    return false;
  }

  int num = mysql_num_rows(res); // 有多少条记录
  resval["status"] = "OK";
  resval["num"] = num;
  if (num == 0) {
    mysql_free_result(res);
    return true;
  }

  for (int i = 0; i < num; i++) {
    MYSQL_ROW row = mysql_fetch_row(res); // 提取一行记录
    Json::Value tmp;
    tmp["tk_id"] = row[0];
    tmp["tk_name"] = row[1];
    tmp["tk_max"] = row[2];
    tmp["tk_num"] = row[3];
    tmp["tk_time"] = row[4];
    resval["tk_arr"].append(tmp);
  }

  mysql_free_result(res);
  return true;
}
bool Mysql_Client::Mysql_Schedule(const string &tel, const string &ticket_id) {
  if (tel.empty() || ticket_id.empty()) {
    return false;
  }

  // 1 查询剩余票数是否大于 0
  // select tk_max, tk_num from ticket_table where tk_id=1;
  string get_num =
      string("select tk_max, tk_num from ticket_table where tk_id=") +
      ticket_id;
  if (mysql_query(&mysql_con, get_num.c_str()) != 0) {
    clog << "get num err" << endl;
    return false;
  }

  MYSQL_RES *res = mysql_store_result(&mysql_con);
  if (res == NULL) {
    cout << "get result err" << endl;
    return false;
  }

  int count = mysql_num_rows(res);
  if (count == 0) {
    cout << "未查询到tk_id" << endl;
    return false;
  }

  MYSQL_ROW row = mysql_fetch_row(res);
  int max = atoi(row[0]);
  int curnum = atoi(row[1]);
  mysql_free_result(res);

  if (max <= curnum) {
    cout << "没有票了" << endl;
    return false;
  }
  // 开始事务
  if (mysql_query(&mysql_con, "begin") != 0) {
    cout << "begin err" << endl;
    return false;
  }

  // 2 num++ ,更新剩余票数
  curnum++;
  // update ticket_table set tk_num=1 where tk_id = 1;
  string set_num = string("update ticket_table set tk_num=") +
                   to_string(curnum) + string(" where tk_id = ") + ticket_id;
  if (mysql_query(&mysql_con, set_num.c_str()) != 0) {
    mysql_query(&mysql_con, "rollback");
    return false;
  }
  // 3 将用户和票id存入yd_table表中
  // insert into yd_table values(0,13700000001,1,now(),1)
  string yd_sql = string("insert into yd_table values(0,") + tel + string(",") +
                  ticket_id + string(",now(),1)");
  if (mysql_query(&mysql_con, yd_sql.c_str()) != 0) {
    mysql_query(&mysql_con, "rollback");
    return false;
  }

  // 提交事务
  mysql_query(&mysql_con, "commit");
  return true;
}
bool Mysql_Client::MySql_Get_TelRegInfo(const string &tel,
                                        Json::Value &resval) {
  // select yd_id ,tk_name,tk_time from yd_table, ticket_table where
  // yd_table.tk_id = ticket_table.tk_id and yd_table.tel=13700000001;
  string sql =
      string("select yd_id ,tk_name,tk_time from yd_table, ticket_table where "
             "yd_table.tk_id = ticket_table.tk_id and yd_table.tel=") +
      tel;
  if (mysql_query(&mysql_con, sql.c_str()) != 0) {
    return false;
  }

  MYSQL_RES *res = mysql_store_result(&mysql_con);
  if (res == nullptr) {
    return false;
  }

  int num = mysql_num_rows(res);
  resval["status"] = "OK";
  resval["num"] = num;

  if (num == 0) {
    mysql_free_result(res);
    return true;
  }

  for (int i = 0; i < num; i++) {
    MYSQL_ROW row = mysql_fetch_row(res);
    Json::Value tmp;
    tmp["yd_id"] = row[0];
    tmp["tk_name"] = row[1];
    tmp["tk_time"] = row[2];
    resval["tk_arr"].append(tmp);
  }

  mysql_free_result(res);
  return true;
}
bool Mysql_Client::Mysql_CancelTelRegister(const string &ticket_id) {
  // select tk_id from yd_table where yd_id=5;
  string get_tkid_sql =
      string("select tk_id from yd_table where yd_id=") + ticket_id;
  if (mysql_query(&mysql_con, get_tkid_sql.c_str()) != 0) {
    return false;
  }

  MYSQL_RES *res = mysql_store_result(&mysql_con);
  if (res == nullptr) {
    return false;
  }

  int num = mysql_num_rows(res);
  if (num == 0) {
    return false;
  }

  MYSQL_ROW row = mysql_fetch_row(res);
  string tk_id = row[0];

  mysql_free_result(res);

  // 开启事务
  if (mysql_query(&mysql_con, "begin") != 0) {
    return false;
  }

  // select tk_num from ticket_table where tk_id=2
  string get_tknum =
      string("select tk_num from ticket_table where tk_id=") + tk_id;
  if (mysql_query(&mysql_con, get_tknum.c_str()) != 0) {
    mysql_query(&mysql_con, "rollback");
    return false;
  }

  res = mysql_store_result(&mysql_con);
  if (res == nullptr) {
    mysql_query(&mysql_con, "rollback");
    return false;
  }

  num = mysql_num_rows(res);
  if (num == 0) {
    mysql_free_result(res);
    mysql_query(&mysql_con, "rollback");
    return false;
  }

  row = mysql_fetch_row(res);
  string tk_num_str = row[0];

  mysql_free_result(res);

  int tk_num = atoi(tk_num_str.c_str());
  if (tk_num <= 0) {
    mysql_query(&mysql_con, "rollback");
    return false;
  }

  tk_num--;

  // update ticket_table set tk_num=1 where tk_id=2
  string set_num = string("update ticket_table set tk_num=") +
                   to_string(tk_num) + string(" where tk_id=") + tk_id;
  if (mysql_query(&mysql_con, set_num.c_str()) != 0) {
    mysql_query(&mysql_con, "rollback");
    return false;
  }

  // delete from yd_table where yd_id=5;
  string del_ydid = string("delete from yd_table where yd_id=") + ticket_id;
  if (mysql_query(&mysql_con, del_ydid.c_str()) != 0) {
    mysql_query(&mysql_con, "rollback");
    return false;
  }

  mysql_query(&mysql_con, "commit");
  return true;
}

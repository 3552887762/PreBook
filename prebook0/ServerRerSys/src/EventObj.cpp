#include "../include/EventObj.hpp"
#include "../include/Logger.hpp"
#include "../include/MyLibevent.hpp"
#include "../include/sqlconn.hpp"
#include <cstring>

#define LIST_MAX 128

bool Socket::Socket_init() {
  m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (m_sockfd == -1) {
    return false;
  }

  struct sockaddr_in saddr;
  memset(&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(m_port);
  saddr.sin_addr.s_addr = inet_addr(m_ips.c_str());

  if (bind(m_sockfd, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
    return false;
  }

  if (listen(m_sockfd, LIST_MAX) == -1) {
    return false;
  }

  return true;
}

void Event_Accept::CallBack_Fun() {
  LOG_TRACE << "IN Event_Accept:: CallBack_Fun";
  int c = accept(sockfd, NULL, NULL);
  if (c < 0) {
    LOG_DEBUG << "accept fail";
    return;
  }

  Event_Recv *r = nullptr;
  LOG_TRACE << "new Event_Recv";
  try {
    r = new Event_Recv(c, plib);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    LOG_DEBUG << "event_recv create error ";
    close(c);
    return;
  }
  LOG_TRACE << "MyLibevent_Add(): " << c;
  if (!plib->MyLibevent_Add(c, r)) {
    delete r;
    LOG_DEBUG << "MyLibevent_Add fail";
    return;
  }
  LOG_TRACE << "OUT Event_Accept:: CallBack_Fun";
}
void Event_Recv::Send_OK() {
  LOG_TRACE << " IN Event_Recv::Send_Ok";
  Json::Value resval;
  resval["status"] = "OK";
  send(c, resval.toStyledString().c_str(), resval.toStyledString().size(), 0);
  LOG_TRACE << " OUT Event_Recv::Send_Ok";
}
void Event_Recv::Send_ERR() {
  LOG_TRACE << " IN Event_Recv::Send_Err";
  Json::Value resval;
  resval["status"] = "ERR";
  send(c, resval.toStyledString().c_str(), resval.toStyledString().size(), 0);
  LOG_TRACE << "OUT Event_Recv::Send_Err";
}
void Event_Recv::Send_Json(Json::Value &val) const {
  LOG_TRACE << "IN Event_Recv::Send_Json";
  send(c, val.toStyledString().c_str(), val.toStyledString().size(), 0);
  LOG_TRACE << " OUT Event_Recv::Send_Json";
}
void Event_Recv::User_zc() {
  string user_tel = m_val["user_tel"].asString();
  string user_name = m_val["user_name"].asString();
  string user_password = m_val["user_password"].asString();

  MysqlClient mysqlcli;
  if (!mysqlcli.ConnectToMysql()) {
    Send_ERR();
    return;
  }

  if (!mysqlcli.Mysql_TelRegister(user_name, user_tel, user_password)) {
    Send_ERR();
    return;
  }

  Send_OK();
  return;
}
void Event_Recv::User_dl() {
  LOG_TRACE << "In Event_Recv::User_Login";
  string user_tel = m_val["user_tel"].asString();
  string user_passwd = m_val["user_password"].asString();
  LOG_TRACE << "tel: " << user_tel;
  LOG_TRACE << "passwd: " << user_passwd;
  if (user_tel.empty() || user_passwd.empty()) {
    LOG_DEBUG << "user_tel.empty || user_passwd.empty";
    Send_ERR();
    return;
  }
  LOG_TRACE << "Create MySql_Client Object";

  MysqlClient cli;
  if (!cli.ConnectToMysql()) {
    LOG_DEBUG << "Connect mySql fail";
    Send_ERR();
    return;
  }
  LOG_TRACE << "cli.MySql_Login:" << user_tel << " " << user_passwd;
  LogInNode node = cli.Mysql_Login(user_tel, user_passwd);
  LOG_TRACE << "node" << node.tel << " " << node.name << " " << node.pass << " "
            << node.tag;
  if (!node.tag) {
    LOG_DEBUG << "Login fail";
    Send_ERR();
    return;
  }

  Json::Value val;
  val["status"] = "OK";
  val["user_name"] = node.name;
  Send_Json(val);
  LOG_TRACE << "OUT Event_Recv::User_Login";
}

void Event_Recv::Show_YuYue() {

  MysqlClient mysqlcli;
  if (!mysqlcli.ConnectToMysql()) {
    Send_ERR();
    return;
  }
  Json::Value resval;
  if (!mysqlcli.Mysql_GetRegisterInfo(resval)) {
    Send_ERR();
    return;
  }

  resval["status"] = "OK";
  Send_Json(resval);
  return;
}

void Event_Recv::User_yd() {
  string user_tel = m_val["user_tel"].asString();
  string tk_id = m_val["tk_id"].asString();

  MysqlClient mysqlcli;
  if (!mysqlcli.ConnectToMysql()) {
    Send_ERR();
    return;
  }

  if (!mysqlcli.Mysql_Schedule(user_tel, tk_id)) {
    Send_ERR();
    return;
  }

  Send_OK();
  return;
}
void Event_Recv::Show_user_yd() {

  string user_tel = m_val["user_tel"].asString();
  MysqlClient cli;
  if (!cli.ConnectToMysql()) {
    Send_ERR();
    return;
  }
  Json::Value resval;
  if (!cli.MySql_Get_TelRegInfo(user_tel, resval)) {
    Send_ERR();
    return;
  }

  resval["status"] = "OK";
  Send_Json(resval);
  return;
}
void Event_Recv::Delete_user_yd() {
  string res_id = m_val["res_id"].asString();

  MysqlClient cli;
  if (!cli.ConnectToMysql()) {
    return;
  }

  if (!cli.Mysql_CancelTelRegister(res_id)) {
    Send_ERR();
    return;
  }

  Send_OK();

  return;
}
void Event_Recv::CallBack_Fun() {
  char buff[256] = {0};
  int n = recv(c, buff, 255, 0); // 接受json数据
  if (n <= 0) {
    plib->MyLibevent_Delete(this);
    delete this;
    return;
  }

  m_val.clear();

  Json::Reader Read;
  if (!Read.parse(buff, m_val)) {
    LOG_TRACE << "json 解析失败";
    Send_ERR();
    return;
  }

  const int User_Op = m_val["type"].asInt();
  switch (User_Op) {
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

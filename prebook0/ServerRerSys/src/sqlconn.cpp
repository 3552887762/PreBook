#include <iostream>
#include "../include/sqlconn.hpp"
#include"../include/Logger.hpp"
using namespace std;
bool MysqlClient::Connect_toDb()
{
    MYSQL *mysql = mysql_init(&mysql_con);
    if (mysql == NULL)
    {
        return false;
    }

    mysql = mysql_real_connect(mysql, ips.c_str(), mysql_username.c_str(), mysql_userpasswd.c_str(), mysql_dbname.c_str(), port, NULL, 0);
    if (mysql == NULL)
    {
        LOG_TRACE << "连接数据库失败" ;
        return false;
    }

    return true;
}
bool MysqlClient::Dd_user_zc(string name, string tel, string pw)
{
    // insert into user_info values(0,'13500000000','小白','111111','1')
    string sql = string("insert into user_info value(0,'") + tel + string("','") + name + string("','") + pw + string("','1')");
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        
        return false;
    }
            LOG_TRACE<<"show user zc success!";
    return true;
}
bool MysqlClient::Dd_user_dl(string &name, string tel, string pw)
{
    // select user_name,user_passwd from user_info where user_tel = 13500000000;
    string sql = string("select user_name,user_passwd from user_info where user_tel = ") + tel;
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        return false;
    }

    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }

    int num = mysql_num_rows(r); //
    if (num != 1)
    {
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(r);
    name = row[0];
    string passwd = row[1];
    if (passwd.compare(pw) != 0)
    {
        return false;
    }
        LOG_TRACE<<"show user dl success!";
    return true;
}

bool MysqlClient::Db_show_yuyue(Json::Value &resval)
{
    // select * from ticket_info
    string sql = string("select * from ticket_info");
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        return false;
    }

    // 提取查询结果
    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }

    int num = mysql_num_rows(r); // 查看结果集有多少行
    resval["num"] = num;
    if (num == 0)
    {
        return true;
    }

    for (int i = 0; i < num; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(r);
        Json::Value tmp;
        tmp["tk_id"] = row[0];
        tmp["tk_name"] = row[1];
        tmp["tk_max"] = row[2];
        tmp["tk_count"] = row[3];
        tmp["tk_date"] = row[4];

        resval["arr"].append(tmp);
    }
    LOG_TRACE<<"show yuyue success!";
    return true;
}
void MysqlClient::Begin()
{
    if (mysql_query(&mysql_con, "begin") != 0)
    {
        LOG_TRACE << "开始事务失败" ;
    }
}
void MysqlClient::RollBack()
{
    if (mysql_query(&mysql_con, "rollback") != 0)
    {
        LOG_TRACE << "回滚失败" ;
    }
}
void MysqlClient::Commit()
{
    if (mysql_query(&mysql_con, "commit") != 0)
    {
        LOG_TRACE << "提交事务失败" ;
    }
}
bool MysqlClient::Db_user_yd(string tel, string tk_id)
{
    // select tk_max,tk_count from ticket_info where tk_id=1;
    string sql_read = string("select tk_max,tk_count from ticket_info where tk_id=") + tk_id;
    if (mysql_query(&mysql_con, sql_read.c_str()) != 0)
    {
        return false;
    }

    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }

    int num = mysql_num_rows(r);
    if (num != 1)
    {
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(r);
    // row[0] row[1]
    int max_num = atoi(row[0]);
    int count = atoi(row[1]);
    if (count >= max_num)
    {
        return false;
    }

    count++;

    Begin();
    // update ticket_info set tk_count=2 where tk_id=1;
    string sql_update = string("update ticket_info set tk_count=") + to_string(count) + string(" where tk_id=") + tk_id;
    if (mysql_query(&mysql_con, sql_update.c_str()) != 0)
    {
        RollBack();
        return false;
    }

    // insert into ticket_res values(0,1,13700000000,now());
    string sql_res = string("insert into ticket_res values(0,") + tk_id + string(",") + tel + string(",now())");
    if (mysql_query(&mysql_con, sql_res.c_str()) != 0)
    {
        RollBack();
        return false;
    }

    Commit();
                LOG_TRACE<<"show user yd success!";
    return true;
}
bool MysqlClient::Db_show_yd(Json::Value &resval, string tel)
{
    // select res_id,tk_name,yd_time from ticket_info, ticket_res where ticket_info.tk_id = ticket_res.tk_id and ticket_res.user_tel = 13500000000;
    string sql = string("select res_id,tk_name,yd_time from ticket_info, ticket_res where ticket_info.tk_id = ticket_res.tk_id and ticket_res.user_tel = ") + tel;
    if (mysql_query(&mysql_con, sql.c_str()) != 0)
    {
        return false;
    }

    // 提取查询结果
    MYSQL_RES *r = mysql_store_result(&mysql_con);
    if (r == NULL)
    {
        return false;
    }

    int num = mysql_num_rows(r); // 查看结果集有多少行
    resval["num"] = num;
    if (num == 0)
    {
        return true;
    }

    for (int i = 0; i < num; i++)
    {
        MYSQL_ROW row = mysql_fetch_row(r);
        Json::Value tmp;
        tmp["res_id"] = row[0];
        tmp["tk_name"] = row[1];
        tmp["yd_time"] = row[2];

        resval["arr"].append(tmp);
    }
                LOG_TRACE<<"show user yd success!";
    return true;
}
bool MysqlClient::Db_Delete_yd(string res_id)
{
    //select tk_id from ticket_res where res_id=3;
    string sql_tkid = string("select tk_id from ticket_res where res_id=")+res_id;
    if ( mysql_query(&mysql_con,sql_tkid.c_str()) != 0 )
    {
        LOG_TRACE<<"查询tk_id失败";
        return false;
    }

    MYSQL_RES * r = mysql_store_result(&mysql_con);
    if ( r == NULL )
    {
        return false;
    }

    int num = mysql_num_rows(r);
    if ( num == 0 )
    {
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(r);
    string tk_id = row[0];
    mysql_free_result(r);

    //select tk_max,tk_count from ticket_info where tk_id=1;
    string sql_count = string("select tk_max,tk_count from ticket_info where tk_id=") + tk_id;
    if ( mysql_query(&mysql_con,sql_count.c_str()) != 0)
    {
        return false;
    }

    r = mysql_store_result(&mysql_con);
    if ( r == NULL )
    {
        return false;
    }

    num = mysql_num_rows(r);
    if ( num != 1 )
    {
        return false;
    }

    row = mysql_fetch_row(r);
    int tk_max = atoi(row[0]);
    int tk_count = atoi(row[1]);
    if ( tk_count > 0 )
    {
        tk_count--;
    }

    mysql_free_result(r);

    Begin();
    //update ticket_info set tk_count=1 where tk_id=1;
    string sql_update = string("update ticket_info set tk_count=")+ to_string(tk_count) + string(" where tk_id=")+tk_id;
    if ( mysql_query(&mysql_con,sql_update.c_str()) != 0)
    {
        RollBack();
        return false;
    }

    //delete from ticket_res where res_id=3
    string sql_del_resid = string("delete from ticket_res where res_id=")+res_id;
    if ( mysql_query(&mysql_con,sql_del_resid.c_str()) != 0)
    {
        RollBack();
        return false;
    }

    Commit();
                LOG_TRACE<<"user delete  success!";
    return true;
}

bool MysqlClient::Db_Delete_yd(string res_id)
{
    //select tk_id from ticket_res where res_id=3;
    string sql_tkid = string("select tk_id from ticket_res where res_id=")+res_id;
    if ( mysql_query(&mysql_con,sql_tkid.c_str()) != 0 )
    {
        cout<<"查询tk_id失败"<<endl;
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
    return true;
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

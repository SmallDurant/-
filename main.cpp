#include <iostream>
#include "pch.h"
using namespace std;
#include "Connection.h"
#include "CommonConnectionPool.h"


int main()
{
   Connection conn;
    char sql[1024] = { 0 };
    sprintf(sql, "insert into tquestion(stem,option,percent,is_delete) values('%s','%s','%s',£¿)", "zhang san", "20", "male",0);
    conn.connect("123.60.152.234", 3306, "chl", "123456", "hundsun");
    conn.update(sql);
    

    
    //clock_t begin = clock();
    /*
    for (int i = 0; i < 1000; ++i) {
        Connection conn;
        char sql[1024] = { 0 };
        sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
        conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
        conn.update(sql);
    }
    */
    /*ConnectionPool* cp = ConnectionPool::getConnectionPool();
    for (int i = 0; i < 1000; ++i) {
        shared_ptr<Connection> sp = cp->getConnection();
        char sql[1024] = { 0 };
        sprintf(sql, "insert into user(name,age,sex) values('%s','%d','%s')", "zhang san", 20, "male");
        sp->update(sql);
    }
    
    clock_t end = clock(); 
    cout << (end - begin) << "ms" << endl;*/
    

    return 0;
}
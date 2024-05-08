#include "pch.h"
#include "CommonConnectionPool.h"
#include "public.h"
//从配置文件中加载配置项
bool ConnectionPool::loadConfigFile()
{
    FILE* pf = fopen("mysql.ini", "r");
    if (pf == nullptr)
    {
        LOG("mysql.ini file is not exist!");
        return false;
    }
    while (!feof(pf))
    {
        char line[1024] = { 0 };
        fgets(line, 1024, pf);
        string str = line;
        int idx = str.find('=', 0);
        if (idx == -1)
        {
            continue;
        }

        //password=123456\n
        int endidx = str.find('\n', idx);
        string key = str.substr(0, idx);
        string value = str.substr(idx + 1, endidx - idx - 1);

        if (key == "ip")
        {
            _ip = value;
        }
        else if (key == "port")
        {
            _port = atoi(value.c_str());
        }
        else if (key == "username")
        {
            _username = value;
        }
        else if (key == "password")
        {
            _password = value;
        }
        else if (key == "dbname")
        {
            _dbname = value;
        }
        else if (key == "initSize")
        {
            _initSize = atoi(value.c_str());
        }
        else if (key == "maxSize")
        {
            _maxSize = atoi(value.c_str());
        }
        else if (key == "maxIdleTime")
        {
            _maxIdleTime = atoi(value.c_str());
        }
        else if (key == "connectionTimeOut")
        {
            _connectionTimeout = atoi(value.c_str());
        }
    }
    return true;
}



//线程安全的懒汉单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool()
{
    static ConnectionPool pool;
    return &pool;
}


ConnectionPool::ConnectionPool()
{
    if (!loadConfigFile())
    {
        return;
    }

    //创建初始数量的连接
    for (int i = 0; i < _initSize; ++i)
    {
        Connection* p = new Connection();
        p->connect(_ip, _port, _username, _password, _dbname);
        p->refreshAliveTime();
        _connectionQue.push(p);
        _connectionCnt++;
    }

    //启动一个新的线程，作为连接的生产者
    thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
    produce.detach();

    //启动一个新的线程扫描超过_maxIdleTime时间的空闲连接，并进行回收
    thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
    scanner.detach();
}

void ConnectionPool::produceConnectionTask()
{
    for (;;)
    {
        unique_lock<mutex> lock(_queueMutex);
        while (!_connectionQue.empty())
        {
            cv.wait(lock);//队列不空，此处生产线程进入等待状态
        }

        //连接数量没有达到上限，继续创建新的连接
        if (_connectionCnt < _maxSize)
        {
            Connection* p = new Connection();
            p->connect(_ip, _port, _username, _password, _dbname);
            p->refreshAliveTime();
            _connectionQue.push(p);
            _connectionCnt++;
        }

        //通知消费者线程，可以消费了
        cv.notify_all();
    }
}

void ConnectionPool::scannerConnectionTask()
{
    for (;;)
    {
        //通过sleep模拟定时效果
        this_thread::sleep_for(chrono::seconds(_maxIdleTime));

        //扫描整个队列，释放多余的连接
        unique_lock<mutex> lock(_queueMutex);
        while (_connectionCnt > _initSize)
        {
            Connection* p = _connectionQue.front();
            if (p->getAliveTime() >= (_maxIdleTime * 1000))
            {
                _connectionQue.pop();
                _connectionCnt--;
                delete p; //释放连接
            }
            else
            {
                break;//对头时间没有超过，那么其他来凝结肯定没有超过
            }
        }
    }
}

shared_ptr<Connection> ConnectionPool::getConnection()
{
    unique_lock<mutex> lock(_queueMutex);
    while (_connectionQue.empty()) 
    {
        if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
        {
            if (_connectionQue.empty())
            {
                LOG("获取空闲连接超时了...获取连接失败！");
                    return nullptr;
            }
        }
    }

    /*
    shared_ptr智能指针析构时，会调用connection的析构函数直接讲connection资源delete掉，
    这里需要自定义释放资源的方式，把connection资源直接归还到queue当中。
    */
    shared_ptr<Connection> sp(_connectionQue.front(),
        [&](Connection* pcon) {
            unique_lock<mutex> lock(_queueMutex);
            pcon->refreshAliveTime();
            _connectionQue.push(pcon);
        });
    _connectionQue.pop();
    cv.notify_all();

    
    return sp;
}


ConnectionPool::~ConnectionPool()
{

}
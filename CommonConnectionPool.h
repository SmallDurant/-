#pragma once
#include <string>
#include <queue>
#include <mutex>
#include <atomic>
#include <memory>
#include <functional>
#include <condition_variable>
#include "Connection.h"
using namespace std;

//ʵ�����ӳع���ģ��
class ConnectionPool
{
public:
	//��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();
	//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
	shared_ptr<Connection> getConnection();


	~ConnectionPool();

private:
	//����#1 ���캯��˽�л�
	ConnectionPool(); 

	//�������ļ��м���������
	bool loadConfigFile();

	//�����ڶ������߳��У�ר�Ÿ�������������
	void produceConnectionTask();

	//ɨ�賬��_maxIdleTimeʱ��Ŀ������ӣ������л���
	void scannerConnectionTask();

	string _ip;           //MySQL��IP��ַ
	unsigned short _port; //MySQL�Ķ˿ں�
	string _username;     //MySQL�ĵ�¼�û���
	string _password;     //MySQL�ĵ�½����
	string _dbname;       //���ӵ����ݿ�����
	int _initSize;		  //���ӳصĳ�ʼ������
	int _maxSize;		  //���ӳص����������
	int _maxIdleTime;	  //���ӳص�������ʱ��
	int _connectionTimeout;//���ӳػ�ȡ���ӵĳ�ʱʱ��

	queue<Connection*> _connectionQue;     //�洢MySQL���ӵĶ���
	mutex _queueMutex;                     //ά�����Ӷ��е��̰߳�ȫ������
	atomic_int _connectionCnt;             //��¼������������connection���ӵ�������
	condition_variable cv;                 //���������������������������̺߳����������̵߳�ͨ��
};



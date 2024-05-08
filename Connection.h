#pragma once
#include <mysql.h>
#include <string>
#include <ctime>
using namespace std;


//ʵ��MySQL���ݿ����ɾ�Ĳ�;

// ���ݿ������
class Connection
{
public:
	// ��ʼ�����ݿ�����
	Connection();

	// �ͷ����ݿ�������Դ
	~Connection();

	// �������ݿ�
	bool connect(string ip, 
		unsigned short port, 
		string user, 
		string password,
		string dbname);

	// ���²��� insert��delete��update
	bool update(string sql);

	// ��ѯ���� select
	MYSQL_RES* query(string sql);

	//ˢ��һ�����ӵ���ʼ�Ŀ���ʱ���
	void refreshAliveTime();
	//���ش��ʱ��
	clock_t getAliveTime();

private:
	MYSQL* _conn; // ��ʾ��MySQL Server��һ������
	clock_t _alivetime;//��¼��ͼ����״̬��Ĵ��ʱ��
};
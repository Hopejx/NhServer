#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<string>
#include<vector>
#include<algorithm>

#include<WinSock2.h>
#include<Windows.h>

using namespace std;
#pragma warning(disable:4996)
#pragma once
//server
enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
//��Ϣͷ
struct DataHeader {
	short dataLength;// ���ݳ���
	short cmd;//����
};
//����
struct LogIn : public DataHeader {
	LogIn() {
		dataLength = sizeof(LogIn);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char PassWord[32];
};
struct LogInResult : public DataHeader {
	LogInResult() {
		dataLength = sizeof(LogInResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};
//�ǳ�
struct LogOut : public DataHeader {
	LogOut() {
		dataLength = sizeof(LogOut);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogOutResult : public DataHeader {
	LogOutResult() {
		dataLength = sizeof(LogOutResult);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

struct NewUserJoin : public DataHeader {
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER_JOIN;
		socket_id = 0;
	}
	int socket_id;
};


/* ֻ����win������
//���� win ��̬���ӿ�
#pragma comment(lib,"ws2_32.lib")
*/

vector<SOCKET> g_clients;
// fun
int PROCESSOR(SOCKET _cSock)
{
	//������
	char szRecv[4096] = {};
	// 5 ���տͻ�������
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("�ͻ������˳������������\n");
		return -1;
	}
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogIn* login = (LogIn*)szRecv;
		printf("�յ����CMD_LOGIN,���ݳ��ȣ�%d,userName=%s PassWord=%s\n", login->dataLength, login->userName, login->PassWord);
		//�����ж��û������Ƿ���ȷ�Ĺ���
		LogInResult ret;
		send(_cSock, (char*)&ret, sizeof(LogInResult), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogOut* logout = (LogOut*)szRecv;
		printf("�յ����CMD_LOGOUT,���ݳ��ȣ�%d,userName=%s \n", logout->dataLength, logout->userName);
		//�����ж��û������Ƿ���ȷ�Ĺ���
		LogOutResult ret;
		send(_cSock, (char*)&ret, sizeof(ret), 0);
	}
	break;
	default:
	{
		DataHeader header = { 0,CMD_ERROR };
		send(_cSock, (char*)&header, sizeof(header), 0);
	}
	break;
	}
	return 0;
}

int main()
{
	//�汾��
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//1. ����һ��Socket
	//ipv4  �ֽ��� tcp����
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//2. bind �����ڽ��ܿͻ������ӵ���·�˿�
	// ����һ��sockaddr �ṹ�� ���socket��Ϣ
	sockaddr_in _sin = {};

	_sin.sin_family = AF_INET;//ָ��ipv4Э�� ipv6 AF_INET6
	_sin.sin_port = htons(4567); // host to net unsigned short

	//sin_addr  ... ��һ�������� ����һ�������׶���
	//INADDR_ANY ���޶�ip��ַ �κ�һ��������
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;// inet_addr("127.0.0.1");

	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof _sin)) {
		cout << "�󶨶˿�ʧ��" << endl;
	}
	else {
		cout << "�󶨶˿ڳɹ�" << endl;
	}

	//3. listen �����˿�
	//���ͬʱ5������
	if (SOCKET_ERROR == listen(_sock, 5)) {
		cout << "�����˿�ʧ��" << endl;
	}
	else {
		cout << "�����˿ڳɹ�" << endl;
	}

	while (true)
	{
		//������ socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		//��FD_ZERO������ʼ�����Ǹ���Ȥ��fd_set��
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		//��FD_SET�������׽��־���������Ӧ��fd_set��
		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);
		
		for (int i = 0; i < g_clients.size(); i++)
		{
			//�ӵ��ɶ���������
			FD_SET(g_clients[i], &fdRead);
		}
		
		//nfds ��һ������ֵ ��ָfd_set����������������(socket)�ķ�Χ������������
		//���������ļ����������ֵ+1 ��Windows�������������д0
		timeval t = { 1,0 };
		//������׽���û��������Ҫ���գ�select������Ѹ��׽��ִӿɶ��Լ�������ɾ������
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			printf("select���������\n");
			break;
		}
		//�ж�_sock �Ƿ���fdRead��
		if (FD_ISSET(_sock, &fdRead))
		{
			//���������������
			FD_CLR(_sock, &fdRead);
			// 4 accept �ȴ����ܿͻ�������
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				printf("����,���ܵ���Ч�ͻ���SOCKET...\n");
			}
			else {
				for (int i = 0; i < g_clients.size(); i++)
				{
					//Ⱥ���������ͻ���
					NewUserJoin userjoin;
					send(g_clients[i], (char*)(&userjoin), sizeof NewUserJoin, 0);
				}
				g_clients.push_back(_cSock);
				printf("�¿ͻ��˼��룺socket = %d,IP = %s \n", (int)_cSock, inet_ntoa(clientAddr.sin_addr));
			}
		}
		for (size_t n = 0; n < fdRead.fd_count; n++)
		{
			if (-1 == PROCESSOR(fdRead.fd_array[n]))
			{
				auto iter = find(g_clients.begin(), g_clients.end(), fdRead.fd_array[n]);
				if (iter != g_clients.end())
				{
					g_clients.erase(iter);
				}
			}
		}
		cout << "����ʱ�䴦������" << endl;
	}

	//8. �ر��׽���
	for (int i = 0; i < g_clients.size(); i++) {
		closesocket(g_clients[i]);
	}

	closesocket(_sock);

	WSACleanup();
	cout << "���˳����������" << endl;
	getchar();
	return 0;
}


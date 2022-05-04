#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>
#include<WinSock2.h>

//#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;
#pragma warning(disable:4996)
#pragma once
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

int PROCESSOR(SOCKET _cSock)
{
	//������
	char szRecv[4096] = {};
	// 5 ���տͻ�������
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("��������Ͽ����ӣ����������\n");
		return -1;
	}
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogInResult* loginresult = (LogInResult*)szRecv;
			printf("�յ����������ݣ�CMD_LOGIN_RESULT,���ݳ��ȣ�%d\n", loginresult->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogOutResult* logoutresult = (LogOutResult*)szRecv;
			printf("�յ����������ݣ�CMD_LOGOUT_RESULT,���ݳ��ȣ�%d\n", logoutresult->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN: 
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin* newuserjoin = (NewUserJoin*)szRecv;
			printf("�յ����������ݣ�CMD_NEW_USER_JOIN,���ݳ��ȣ�%d\n", newuserjoin->dataLength);
		}
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
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _socket) {
		cout << "���󣬽���Socketʧ��" << endl;
	}
	else {
		cout << "�ɹ�������Socket�ɹ�" << endl;
	}

	//2. ���ӷ����� connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (INVALID_SOCKET == connect(_socket, (sockaddr*)(&_sin), sizeof _sin)) {
		cout << "���ӷ�����ʧ��" << endl;
	}
	else {
		cout << "���ӷ������ɹ�" << endl;
	}

	char cmdBuff[128] = {};
	while (true) {
		fd_set fdReads;
		//��ʼ��
		FD_ZERO(&fdReads);
		//��SOCKET �����fdReads
		FD_SET(_socket,&fdReads);

		//fd_set fdWrite;
		//fd_set fdError;
		timeval t = { 1, 0 };
		int ret = select(_socket + 1, &fdReads, 0, 0, &t);
		if (ret < 0) {
			cout << "�������" << endl;
			break;
		}
		if (FD_ISSET(_socket, &fdReads)) {
			//�ܵ���˵���ǣ�ɾ������set���׽���fd
			FD_CLR(_socket, &fdReads);
			if (-1 == PROCESSOR(_socket)) {
				cout << "select�������" << endl;
				break;
			}
		}
		LogIn login;
		strcpy(login.userName, "hope");
		strcpy(login.PassWord, "123456");
		Sleep(1000);
		send(_socket, (char *)(&login), sizeof LogIn, 0);
		cout << "����ʱ�䴦������ҵ��" << endl;
	}

	//7. �ر��׽���
	closesocket(_socket);
	WSACleanup();
	cout << "���˳����������" << endl;
	getchar();
	return 0;
}


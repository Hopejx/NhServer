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
//消息头
struct DataHeader {
	short dataLength;// 数据长度
	short cmd;//命令
};
//登入
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
//登出
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
	//缓冲区
	char szRecv[4096] = {};
	// 5 接收客户端数据
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("与服务器断开连接，任务结束。\n");
		return -1;
	}
	switch (header->cmd)
	{
		case CMD_LOGIN_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogInResult* loginresult = (LogInResult*)szRecv;
			printf("收到服务器数据：CMD_LOGIN_RESULT,数据长度：%d\n", loginresult->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			LogOutResult* logoutresult = (LogOutResult*)szRecv;
			printf("收到服务器数据：CMD_LOGOUT_RESULT,数据长度：%d\n", logoutresult->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN: 
		{
			recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
			NewUserJoin* newuserjoin = (NewUserJoin*)szRecv;
			printf("收到服务器数据：CMD_NEW_USER_JOIN,数据长度：%d\n", newuserjoin->dataLength);
		}
	}
	return 0;
}


int main()
{
	//版本号
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//1. 建立一个Socket
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _socket) {
		cout << "错误，建立Socket失败" << endl;
	}
	else {
		cout << "成功，建立Socket成功" << endl;
	}

	//2. 连接服务器 connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (INVALID_SOCKET == connect(_socket, (sockaddr*)(&_sin), sizeof _sin)) {
		cout << "连接服务器失败" << endl;
	}
	else {
		cout << "连接服务器成功" << endl;
	}

	char cmdBuff[128] = {};
	while (true) {
		fd_set fdReads;
		//初始化
		FD_ZERO(&fdReads);
		//将SOCKET 分配给fdReads
		FD_SET(_socket,&fdReads);

		//fd_set fdWrite;
		//fd_set fdError;
		timeval t = { 1, 0 };
		int ret = select(_socket + 1, &fdReads, 0, 0, &t);
		if (ret < 0) {
			cout << "任务结束" << endl;
			break;
		}
		if (FD_ISSET(_socket, &fdReads)) {
			//总的来说就是，删除集合set中套接字fd
			FD_CLR(_socket, &fdReads);
			if (-1 == PROCESSOR(_socket)) {
				cout << "select任务结束" << endl;
				break;
			}
		}
		LogIn login;
		strcpy(login.userName, "hope");
		strcpy(login.PassWord, "123456");
		Sleep(1000);
		send(_socket, (char *)(&login), sizeof LogIn, 0);
		cout << "空闲时间处理其他业务" << endl;
	}

	//7. 关闭套接字
	closesocket(_socket);
	WSACleanup();
	cout << "已退出，任务结束" << endl;
	getchar();
	return 0;
}


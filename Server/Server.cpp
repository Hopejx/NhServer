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


/* 只能在win条件下
//调用 win 静态链接库
#pragma comment(lib,"ws2_32.lib")
*/

vector<SOCKET> g_clients;
// fun
int PROCESSOR(SOCKET _cSock)
{
	//缓冲区
	char szRecv[4096] = {};
	// 5 接收客户端数据
	int nLen = recv(_cSock, szRecv, sizeof(DataHeader), 0);
	DataHeader* header = (DataHeader*)szRecv;
	if (nLen <= 0)
	{
		printf("客户端已退出，任务结束。\n");
		return -1;
	}
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogIn* login = (LogIn*)szRecv;
		printf("收到命令：CMD_LOGIN,数据长度：%d,userName=%s PassWord=%s\n", login->dataLength, login->userName, login->PassWord);
		//忽略判断用户密码是否正确的过程
		LogInResult ret;
		send(_cSock, (char*)&ret, sizeof(LogInResult), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		recv(_cSock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);
		LogOut* logout = (LogOut*)szRecv;
		printf("收到命令：CMD_LOGOUT,数据长度：%d,userName=%s \n", logout->dataLength, logout->userName);
		//忽略判断用户密码是否正确的过程
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
	//版本号
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//1. 建立一个Socket
	//ipv4  字节流 tcp类型
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//2. bind 绑定用于接受客户端连接的网路端口
	// 创建一个sockaddr 结构体 存放socket信息
	sockaddr_in _sin = {};

	_sin.sin_family = AF_INET;//指定ipv4协议 ipv6 AF_INET6
	_sin.sin_port = htons(4567); // host to net unsigned short

	//sin_addr  ... 是一个联合体 不是一个对象套对象
	//INADDR_ANY 不限定ip地址 任何一个都可以
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;// inet_addr("127.0.0.1");

	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof _sin)) {
		cout << "绑定端口失败" << endl;
	}
	else {
		cout << "绑定端口成功" << endl;
	}

	//3. listen 侦听端口
	//最大同时5个连接
	if (SOCKET_ERROR == listen(_sock, 5)) {
		cout << "监听端口失败" << endl;
	}
	else {
		cout << "监听端口成功" << endl;
	}

	while (true)
	{
		//伯克利 socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		//用FD_ZERO宏来初始化我们感兴趣的fd_set。
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		//用FD_SET宏来将套接字句柄分配给相应的fd_set。
		FD_SET(_sock, &fdRead);
		FD_SET(_sock, &fdWrite);
		FD_SET(_sock, &fdExp);
		
		for (int i = 0; i < g_clients.size(); i++)
		{
			//加到可读序列里面
			FD_SET(g_clients[i], &fdRead);
		}
		
		//nfds 是一个整数值 是指fd_set集合中所有描述符(socket)的范围，而不是数量
		//既是所有文件描述符最大值+1 在Windows中这个参数可以写0
		timeval t = { 1,0 };
		//如果该套接字没有数据需要接收，select函数会把该套接字从可读性检查队列中删除掉，
		int ret = select(_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0)
		{
			printf("select任务结束。\n");
			break;
		}
		//判断_sock 是否在fdRead中
		if (FD_ISSET(_sock, &fdRead))
		{
			//清除监听的描述符
			FD_CLR(_sock, &fdRead);
			// 4 accept 等待接受客户端连接
			sockaddr_in clientAddr = {};
			int nAddrLen = sizeof(sockaddr_in);
			SOCKET _cSock = INVALID_SOCKET;
			_cSock = accept(_sock, (sockaddr*)&clientAddr, &nAddrLen);
			if (INVALID_SOCKET == _cSock)
			{
				printf("错误,接受到无效客户端SOCKET...\n");
			}
			else {
				for (int i = 0; i < g_clients.size(); i++)
				{
					//群发给其他客户端
					NewUserJoin userjoin;
					send(g_clients[i], (char*)(&userjoin), sizeof NewUserJoin, 0);
				}
				g_clients.push_back(_cSock);
				printf("新客户端加入：socket = %d,IP = %s \n", (int)_cSock, inet_ntoa(clientAddr.sin_addr));
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
		cout << "空闲时间处理其他" << endl;
	}

	//8. 关闭套接字
	for (int i = 0; i < g_clients.size(); i++) {
		closesocket(g_clients[i]);
	}

	closesocket(_sock);

	WSACleanup();
	cout << "已退出，任务结束" << endl;
	getchar();
	return 0;
}


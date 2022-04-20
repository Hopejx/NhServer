#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>
#include<WinSock2.h>
using namespace std;

/* 只能在win条件下
//调用 win 静态链接库
#pragma comment(lib,"ws2_32.lib")
*/


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
		cout << "绑定失败" << endl;
	}
	else {
		cout << "绑定成功" << endl;
	}

	//3. listen 侦听端口
	//最大同时5个连接
	if (SOCKET_ERROR == listen(_sock, 5)) {
		cout << "监听失败" << endl;
	}
	else {
		cout << "监听成功" << endl;
	}
	//4 accept 等待接受客户端连接

	//这个socket接受的是客户端的信息
	sockaddr_in _clientAddr = { };
	//int nAddrLen = sizeof sockaddr_in;
	//accept(_sock, (sockaddr*)&_clientAddr, &nAddrLen);
	//接受客户端的信息 socket 存到这个socket里面
	SOCKET _clientSocket = INVALID_SOCKET;

	char buff[] = "hello client,I am Server";

	while (true) {
		_clientSocket = accept(_sock, (sockaddr*)&_clientAddr, (int*)(sizeof _clientAddr));
		if (INVALID_SOCKET == _clientSocket) {
			cout << "接收到无效客户端socket" << endl;
		}
		else {
			//cout << "接收到有效客户端socket" << endl;
		}

		//cout << "新客户端加入" << "IP为：" << inet_ntoa(_clientAddr.sin_addr) << endl;
		//5. send 向客户端发送一条数据
		send(_clientSocket, buff, strlen(buff) + 1, 0);
	}
	//6. 关闭套接字
	closesocket(_sock);

	WSACleanup();
	return 0;
}


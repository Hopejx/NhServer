#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>
#include<WinSock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;


int main()
{
	//版本号
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//1. 建立一个Socket
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _socket) {
		cout << "错误，建立失败" << endl;
	}
	else {
		cout << "成功，建立成功" << endl;
	}

	//2. 连接服务器 connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (INVALID_SOCKET == connect(_socket, (sockaddr*)(&_sin), sizeof _sin)) {
		cout << "连接失败" << endl;
	}
	else {
		cout << "连接成功" << endl;
	}
	//3. 接受服务器信息
	char rebuff[256] = {};
	//调用recv
	int nlen = recv(_socket, rebuff, 256, 0);
	if (nlen > 0) {
		cout << "接受到数据" << endl;
		printf("%s\n", rebuff);
	}
	//4. 关闭套接字
	closesocket(_socket);
	WSACleanup();
	return 0;
}


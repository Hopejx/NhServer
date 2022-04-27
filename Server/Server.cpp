#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>
#include<string>
#include<WinSock2.h>
using namespace std;
#pragma warning(disable:4996)
/* 只能在win条件下
//调用 win 静态链接库
#pragma comment(lib,"ws2_32.lib")
*/
enum CMD {
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};

//消息头
struct DataHeader {
	short dataLength;// 数据长度
	short cmd;//命令
};
//登入
struct LogIn {
	char userName[32];
	char PassWord[32];
};
struct LogInResule {
	int result;
};
//登出
struct LogOut {
	char userName[32];
};

struct LogOutResule {
	int result;
};

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

	//4 accept 等待接受客户端连接
	//这个socket接受的是客户端的信息
	sockaddr_in _clientAddr = { };
	int nAddrLen = sizeof _clientAddr;
	//int nAddrLen = sizeof sockaddr_in;
	//accept(_sock, (sockaddr*)&_clientAddr, &nAddrLen);
	//接受客户端的信息 socket 存到这个socket里面
	SOCKET _clientSocket = INVALID_SOCKET;
	char buff[] = "hello client,I am Server";

	_clientSocket = accept(_sock, (sockaddr*)&_clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _clientSocket) {
		cout << "接收到无效客户端socket" << endl;
	}
	cout << "新客户端加入" << "IP为：" << inet_ntoa(_clientAddr.sin_addr) << endl;
	char Rebuff[128] = {};

	while (true) {
		DataHeader header = {};

		// 5. 接收客户端发送的请求
		int nLen = recv(_clientSocket, (char *)&header, sizeof DataHeader, 0);
		
		if (nLen <= 0) {
			cout << "客户端已退出,任务结束" << endl;
			break;
		}
		cout << "收到命令：" << header.cmd << "收到数据长度：" << header.dataLength<<endl;
		
		
		//6. 处理请求
		switch (header.cmd) {
			case CMD_LOGIN: {
				LogIn login = {};
				recv(_clientSocket, (char *)&login, sizeof LogIn, 0);
				//忽略判断用户密码是否正确的过程
				LogInResule ret = { 0 };

				send(_clientSocket, (char *)&header, sizeof DataHeader, 0);
				send(_clientSocket, (char *)&ret, sizeof LogInResule, 0);
			
			}
			break;
			case CMD_LOGOUT: {
				LogOut logout = {};
				recv(_clientSocket, (char *)&logout, sizeof LogOut, 0);
				//忽略判断用户密码是否正确的过程
				LogOutResule ret = { 0 };

				send(_clientSocket, (char *)&header, sizeof DataHeader, 0);
				send(_clientSocket, (char *)&ret, sizeof LogOutResule, 0);
			}
			break;
			default: 
				header.cmd = CMD_ERROR;
				header.dataLength = 0;
				send(_clientSocket, (char *)&header, sizeof DataHeader, 0);
			
			break;
		}

	}

	//8. 关闭套接字
	closesocket(_sock);

	WSACleanup();
	cout << "已退出，任务结束" << endl;
	getchar();
	return 0;
}


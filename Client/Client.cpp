#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>
#include<WinSock2.h>
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;
#pragma warning(disable:4996)

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
		//3. 输入请求命令
		scanf("%s", &cmdBuff);
		//4. 处理请求命令
		if (0 == strcmp(cmdBuff, "exit")) {
			cout << "循环结束" << endl;
			break;
		}
		else if (0 == strcmp(cmdBuff, "login")) {
			LogIn Login = { "hope","123456" };
			DataHeader dh = { sizeof(Login) ,CMD_LOGIN };
			
			//先发数据头
			send(_socket, (const char *)&dh, sizeof(dh), 0);
			send(_socket, (const char *)&Login, sizeof (Login), 0);
			//接受服务器返回的数据
			DataHeader retHeaderin = {};
			LogInResule Loginresult = {};
			recv(_socket, (char*)&retHeaderin, sizeof retHeaderin, 0);
			recv(_socket, (char*)&Loginresult, sizeof Loginresult, 0);
			cout << "LoginResult:" << Loginresult.result << endl;
		}
		else if (0 == strcmp(cmdBuff, "logout")) {
			LogOut logout = { "hope" };
			DataHeader dh = {sizeof logout ,CMD_LOGOUT };
			//向服务器发送
			send(_socket, (char *)&dh, sizeof(dh), 0);
			send(_socket, (char *)&logout, sizeof (logout), 0);
			//接收
			DataHeader retHeaderout = {};
			LogOutResule Logoutresult = {};
			recv(_socket, (char *)&retHeaderout, sizeof retHeaderout, 0);
			recv(_socket, (char *)&Logoutresult, sizeof Logoutresult, 0);
			cout << "LoginResult:" << Logoutresult.result << endl;
		}
		else {
			cout << "不支持的命令：重新输入" << endl;
		}
	}

	//7. 关闭套接字
	closesocket(_socket);
	WSACleanup();
	cout << "已退出，任务结束" << endl;
	getchar();
	return 0;
}


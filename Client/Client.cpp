#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>
#include<WinSock2.h>
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;
#pragma warning(disable:4996)

enum CMD {
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
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

struct LogOutResule : public DataHeader {
	LogOutResule() {
		dataLength = sizeof(LogOutResule);
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
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
			LogIn Login;
			strcpy(Login.userName,"hope");
			strcpy(Login.PassWord, "123456");
			//先发数据头
			send(_socket, (const char *)&Login, sizeof (Login), 0);
			//接受服务器返回的数据
			LogInResult Loginresult = {};
			recv(_socket, (char*)&Loginresult, sizeof Loginresult, 0);
			cout << "LoginResult:" << Loginresult.result << endl;
		}
		else if (0 == strcmp(cmdBuff, "logout")) {
			LogOut logout;
			strcpy(logout.userName, "hope");
			//向服务器发送
			send(_socket, (char *)&logout, sizeof (logout), 0);
			//接收
			LogOutResule Logoutresult = {};
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


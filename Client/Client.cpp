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
		//3. ������������
		scanf("%s", &cmdBuff);
		//4. ������������
		if (0 == strcmp(cmdBuff, "exit")) {
			cout << "ѭ������" << endl;
			break;
		}
		else if (0 == strcmp(cmdBuff, "login")) {
			LogIn Login;
			strcpy(Login.userName,"hope");
			strcpy(Login.PassWord, "123456");
			//�ȷ�����ͷ
			send(_socket, (const char *)&Login, sizeof (Login), 0);
			//���ܷ��������ص�����
			LogInResult Loginresult = {};
			recv(_socket, (char*)&Loginresult, sizeof Loginresult, 0);
			cout << "LoginResult:" << Loginresult.result << endl;
		}
		else if (0 == strcmp(cmdBuff, "logout")) {
			LogOut logout;
			strcpy(logout.userName, "hope");
			//�����������
			send(_socket, (char *)&logout, sizeof (logout), 0);
			//����
			LogOutResule Logoutresult = {};
			recv(_socket, (char *)&Logoutresult, sizeof Logoutresult, 0);
			cout << "LoginResult:" << Logoutresult.result << endl;
		}
		else {
			cout << "��֧�ֵ������������" << endl;
		}
	}

	//7. �ر��׽���
	closesocket(_socket);
	WSACleanup();
	cout << "���˳����������" << endl;
	getchar();
	return 0;
}


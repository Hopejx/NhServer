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
//��Ϣͷ
struct DataHeader {
	short dataLength;// ���ݳ���
	short cmd;//����
};
//����
struct LogIn {
	char userName[32];
	char PassWord[32];
};
struct LogInResule {
	int result;
};
//�ǳ�
struct LogOut {
	char userName[32];
};

struct LogOutResule {
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
			LogIn Login = { "hope","123456" };
			DataHeader dh = { sizeof(Login) ,CMD_LOGIN };
			
			//�ȷ�����ͷ
			send(_socket, (const char *)&dh, sizeof(dh), 0);
			send(_socket, (const char *)&Login, sizeof (Login), 0);
			//���ܷ��������ص�����
			DataHeader retHeaderin = {};
			LogInResule Loginresult = {};
			recv(_socket, (char*)&retHeaderin, sizeof retHeaderin, 0);
			recv(_socket, (char*)&Loginresult, sizeof Loginresult, 0);
			cout << "LoginResult:" << Loginresult.result << endl;
		}
		else if (0 == strcmp(cmdBuff, "logout")) {
			LogOut logout = { "hope" };
			DataHeader dh = {sizeof logout ,CMD_LOGOUT };
			//�����������
			send(_socket, (char *)&dh, sizeof(dh), 0);
			send(_socket, (char *)&logout, sizeof (logout), 0);
			//����
			DataHeader retHeaderout = {};
			LogOutResule Logoutresult = {};
			recv(_socket, (char *)&retHeaderout, sizeof retHeaderout, 0);
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


#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>
#include<WinSock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;


int main()
{
	//�汾��
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//1. ����һ��Socket
	SOCKET _socket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _socket) {
		cout << "���󣬽���ʧ��" << endl;
	}
	else {
		cout << "�ɹ��������ɹ�" << endl;
	}

	//2. ���ӷ����� connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (INVALID_SOCKET == connect(_socket, (sockaddr*)(&_sin), sizeof _sin)) {
		cout << "����ʧ��" << endl;
	}
	else {
		cout << "���ӳɹ�" << endl;
	}
	//3. ���ܷ�������Ϣ
	char rebuff[256] = {};
	//����recv
	int nlen = recv(_socket, rebuff, 256, 0);
	if (nlen > 0) {
		cout << "���ܵ�����" << endl;
		printf("%s\n", rebuff);
	}
	//4. �ر��׽���
	closesocket(_socket);
	WSACleanup();
	return 0;
}


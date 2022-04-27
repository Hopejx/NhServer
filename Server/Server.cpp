#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>
#include<string>
#include<WinSock2.h>
using namespace std;
#pragma warning(disable:4996)
/* ֻ����win������
//���� win ��̬���ӿ�
#pragma comment(lib,"ws2_32.lib")
*/
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
	//ipv4  �ֽ��� tcp����
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//2. bind �����ڽ��ܿͻ������ӵ���·�˿�
	// ����һ��sockaddr �ṹ�� ���socket��Ϣ
	sockaddr_in _sin = {};

	_sin.sin_family = AF_INET;//ָ��ipv4Э�� ipv6 AF_INET6
	_sin.sin_port = htons(4567); // host to net unsigned short

	//sin_addr  ... ��һ�������� ����һ�������׶���
	//INADDR_ANY ���޶�ip��ַ �κ�һ��������
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;// inet_addr("127.0.0.1");

	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof _sin)) {
		cout << "�󶨶˿�ʧ��" << endl;
	}
	else {
		cout << "�󶨶˿ڳɹ�" << endl;
	}

	//3. listen �����˿�
	//���ͬʱ5������
	if (SOCKET_ERROR == listen(_sock, 5)) {
		cout << "�����˿�ʧ��" << endl;
	}
	else {
		cout << "�����˿ڳɹ�" << endl;
	}

	//4 accept �ȴ����ܿͻ�������
	//���socket���ܵ��ǿͻ��˵���Ϣ
	sockaddr_in _clientAddr = { };
	int nAddrLen = sizeof _clientAddr;
	//int nAddrLen = sizeof sockaddr_in;
	//accept(_sock, (sockaddr*)&_clientAddr, &nAddrLen);
	//���ܿͻ��˵���Ϣ socket �浽���socket����
	SOCKET _clientSocket = INVALID_SOCKET;
	char buff[] = "hello client,I am Server";

	_clientSocket = accept(_sock, (sockaddr*)&_clientAddr, &nAddrLen);
	if (INVALID_SOCKET == _clientSocket) {
		cout << "���յ���Ч�ͻ���socket" << endl;
	}
	cout << "�¿ͻ��˼���" << "IPΪ��" << inet_ntoa(_clientAddr.sin_addr) << endl;
	char Rebuff[128] = {};

	while (true) {
		DataHeader header = {};

		// 5. ���տͻ��˷��͵�����
		int nLen = recv(_clientSocket, (char *)&header, sizeof DataHeader, 0);
		
		if (nLen <= 0) {
			cout << "�ͻ������˳�,�������" << endl;
			break;
		}
		cout << "�յ����" << header.cmd << "�յ����ݳ��ȣ�" << header.dataLength<<endl;
		
		
		//6. ��������
		switch (header.cmd) {
			case CMD_LOGIN: {
				LogIn login = {};
				recv(_clientSocket, (char *)&login, sizeof LogIn, 0);
				//�����ж��û������Ƿ���ȷ�Ĺ���
				LogInResule ret = { 0 };

				send(_clientSocket, (char *)&header, sizeof DataHeader, 0);
				send(_clientSocket, (char *)&ret, sizeof LogInResule, 0);
			
			}
			break;
			case CMD_LOGOUT: {
				LogOut logout = {};
				recv(_clientSocket, (char *)&logout, sizeof LogOut, 0);
				//�����ж��û������Ƿ���ȷ�Ĺ���
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

	//8. �ر��׽���
	closesocket(_sock);

	WSACleanup();
	cout << "���˳����������" << endl;
	getchar();
	return 0;
}


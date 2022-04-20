#include <iostream>
#define WIN32_LEAN_AND_MEAN 
#include<Windows.h>
#include<WinSock2.h>
using namespace std;

/* ֻ����win������
//���� win ��̬���ӿ�
#pragma comment(lib,"ws2_32.lib")
*/


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
		cout << "��ʧ��" << endl;
	}
	else {
		cout << "�󶨳ɹ�" << endl;
	}

	//3. listen �����˿�
	//���ͬʱ5������
	if (SOCKET_ERROR == listen(_sock, 5)) {
		cout << "����ʧ��" << endl;
	}
	else {
		cout << "�����ɹ�" << endl;
	}
	//4 accept �ȴ����ܿͻ�������

	//���socket���ܵ��ǿͻ��˵���Ϣ
	sockaddr_in _clientAddr = { };
	//int nAddrLen = sizeof sockaddr_in;
	//accept(_sock, (sockaddr*)&_clientAddr, &nAddrLen);
	//���ܿͻ��˵���Ϣ socket �浽���socket����
	SOCKET _clientSocket = INVALID_SOCKET;

	char buff[] = "hello client,I am Server";

	while (true) {
		_clientSocket = accept(_sock, (sockaddr*)&_clientAddr, (int*)(sizeof _clientAddr));
		if (INVALID_SOCKET == _clientSocket) {
			cout << "���յ���Ч�ͻ���socket" << endl;
		}
		else {
			//cout << "���յ���Ч�ͻ���socket" << endl;
		}

		//cout << "�¿ͻ��˼���" << "IPΪ��" << inet_ntoa(_clientAddr.sin_addr) << endl;
		//5. send ��ͻ��˷���һ������
		send(_clientSocket, buff, strlen(buff) + 1, 0);
	}
	//6. �ر��׽���
	closesocket(_sock);

	WSACleanup();
	return 0;
}


//Socket�࣬��������
#pragma once

class Socket
{
private:
	SOCKET RTSPSocket;
	char *IPAddr;
	char *Port;

public:
	WSADATA wsaData;

	Socket();
	~Socket();

	int initWinsock(char *dotIPAddr, char *RTSPPort);
	int connectServer();
	//int sendMsg(char *msg);
	//int recvMsg(char *msg);

	SOCKET GetSocket();
};
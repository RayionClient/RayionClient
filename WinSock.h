//Socket类，用于连接
#pragma once

class Socket
{
private:
	SOCKET RTSPSocket;
	char *IPAddr;
	char *Port;

public:
	Socket();
	~Socket();

	int initWinsock(char *dotIPAddr, char *RTSPPort);
	int connectServer();
	//int sendMsg(char *msg);
	//int recvMsg(char *msg);

	SOCKET GetSocket();
};
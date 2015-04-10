#include "stdafx.h"
#include "WinSock.h"

Socket::Socket()
{
	RTSPSocket = socket(AF_INET, SOCK_STREAM, 0);

	IPAddr = new char[18];
	Port = new char[6];
}

Socket::~Socket()
{
	shutdown(RTSPSocket, SD_BOTH);
	closesocket(RTSPSocket);
	WSACleanup();
}

int Socket::initWinsock(char *dotIPAddr, char *RTSPPort)
{
	IPAddr = dotIPAddr;
	Port = RTSPPort;

	return 0;
}

int Socket::connectServer()
{
	in_addr address;
	sockaddr_in ServerAddr;

	ServerAddr.sin_family = AF_INET;
	inet_pton(AF_INET, IPAddr, &address);
	ServerAddr.sin_addr = address;
	ServerAddr.sin_port = htons(atoi(Port));

	int flag = connect(RTSPSocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr));//这里是可能超时的，默认超时时间是75秒
	return flag;
}

SOCKET Socket::GetSocket()
{
	return RTSPSocket;
}
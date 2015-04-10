#include <iostream>
#include <stdlib.h>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>


//jrtplib
#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"

#pragma comment(lib, "Ws2_32.lib")

#ifndef DEBUG
#pragma comment(lib, "jrtplib_d.lib") 
#pragma comment(lib,"jthread_d.lib")
#else
#pragma comment(lib, "jrtplib.lib") 
#pragma comment(lib,"jthread.lib")
#endif

using namespace jrtplib;
using namespace std;

//RTSP����
enum RTSPmessage
{
	OPTION = 1,
	DESCRIBE = 2,
	SETUP = 3,
	PLAY = 4,
	PAUSE = 5,
	STOP = 6,
	TEARDOWN = 7,
	GET_PARAMETER = 8,

	//�������
};

enum CustomDefine
{
	BUF_SIZE = 8192,
};

// �ź���
HANDLE hEvent_RTP = CreateEvent(NULL, TRUE, FALSE, NULL);;         // ���ƿ�ʼ����RTP��
HANDLE hMutex_Play = CreateMutex(NULL, FALSE, NULL);               // ���Ʋ���ˢ��
HANDLE hEvent_Close = CreateEvent(NULL, TRUE, FALSE, NULL);;       // ֪ͨ���߳��˳�
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

//RTSP信令
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

	//错误代码
};

enum CustomDefine
{
	BUF_SIZE = 8192,
};

// 信号量
HANDLE hEvent_RTP = CreateEvent(NULL, TRUE, FALSE, NULL);;         // 控制开始接收RTP包
HANDLE hMutex_Play = CreateMutex(NULL, FALSE, NULL);               // 控制播放刷新
HANDLE hEvent_Close = CreateEvent(NULL, TRUE, FALSE, NULL);;       // 通知各线程退出
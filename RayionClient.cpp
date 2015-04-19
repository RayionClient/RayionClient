//==================================//
//=====      Rayion Tech.      =====//
//                                  //
// RTSP Client For Remote Rendering //
//                                  //
//     Written by Li Hong-jie       //
//           2015-04-08             //
//                                  //
//Under protection of some licenses.//
//==================================//

#include "stdafx.h"
#include "URIParsing.h"
#include "WinSock.h"
#include "RTSPmsg.h"
#include "RTPHandler.h"
#include "RayionClient.h"


int main(int argc, char *argv[])
{
	argc = 4;
	argv[1] = "rtsp://192.168.1.115:554/live";
	argv[2] = "Reserved for login user name";
	argv[3] = "Reserved for login pwd";

	//很多功能都要初始化Winsock，所以单独拿出来初始化
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	URIInfo uri;                // URI信息，存储和解析URI
	Socket sk;                  // Winsock连接相关1

	//=====输入合法性判断与URI解析========
	if (argc != 4)
	{
		//参数数目不对，报错

		return 0;
	}
	
	uri.initURIInfo(argv[1]);
	//====================================

	//=======使用Winsock与服务器创建好连接=====
	sk.initWinsock(uri.GetDotIPAddr(), uri.GetstrPort());
	int cnct = sk.connectServer();      //成功连接则返回零

	if (cnct != 0)
	{
		cout << "Failed to connect the server.";
		return 0;
	}
	//========================================

	//创建线程
	rtspParam rtspPara;
	rtspPara.URI = argv[1];
	rtspPara.Port = uri.GetintPort();
	rtspPara.socket = sk.GetSocket();

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rtspControl, &rtspPara, 0, NULL);       //RTSP控制线程

	rtpParam rtpPara;
	rtpPara.IPAddr = uri.GetIPAddr();
	rtpPara.Port = uri.GetintPort();

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rtpHandler, &rtpPara, 0, NULL);


	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DecoderAndPlayer, NULL, 0, NULL);

	while (1)
	{
		//
	}

	return 0;
}


//RTSP控制线程
UINT rtspControl(LPVOID lpParam)
{
	rtspParam *param = (rtspParam *)lpParam;
	RTSPmsg rtspMsg;            // RTSP信令相关
	int flag;

	//发送RTSP信令，并对答令进行相应解析
	rtspMsg.initRTSPmsg(param->URI, param->Port, param->RTPPort, param->socket);     // 初始化
	flag = rtspMsg.play();                                           // 播放，即完成从OPTION到PLAY的信令发送工作
	if (flag != 200)
	{
		cout << "Error in playing media." << endl;
		return 0;
	}

	// 心跳
	int timer = 0;
	while (1)
	{
		timer++;           //计时器
		
		if (timer > 100000)
		{
			timer = 0;
			//rtspMsg.HeartBeat();
		}

		////事件控制结束
		//if (WaitForSingleObject(hEvent_Close, 1) == WAIT_OBJECT_0)
		//{
		//	break;
		//}
	}

	return 0;
}

//RTP收包线程
UINT rtpHandler(LPVOID lpParam)
{
	rtpParam *rParam = (rtpParam *)lpParam;
	RTPHandler rtpHandler;

	rtpHandler.initRTPHandler(rParam->IPAddr, rParam->RTPPort);
	rtpHandler.recvPackets();

	//事件控制结束
	//WaitForSingleObject(hEvent_Close, INFINITE);

	return 0;
}
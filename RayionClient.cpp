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

	URIInfo uri;                // URI信息，存储和解析URI
	Socket sk;                  // Winsock连接相关
	
	RTPHandler rtp;             // 使用JRTPLIB处理RTP包

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
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DecoderAndPlayer, NULL, 0, NULL);


	



	//	//使用jrtplib接收数据包
	//	RTPSession session;//The constructor accepts two parameter, an instance of an RTPRandom object, and an instance of an RTPMemoryManager object.

	//	RTPSessionParams sessionparams;
	//	sessionparams.SetOwnTimestampUnit(1.0 / 8000.0);

	//	RTPUDPv4TransmissionParams transparams;
	//	transparams.SetPortbase(8000);

	//	int status = session.Create(sessionparams, &transparams);
	//	if (status < 0)
	//	{
	//		std::cerr << RTPGetErrorString(status) << std::endl;
	//		exit(-1);
	//	}

	//	uint8_t localip[] = { 127, 0, 0, 1 };
	//	RTPIPv4Address addr(localip, 80);

	//	status = session.AddDestination(addr);
	//	if (status < 0)
	//	{
	//		std::cerr << RTPGetErrorString(status) << std::endl;
	//		exit(-1);
	//	}

	//	session.SetDefaultPayloadType(96);
	//	session.SetDefaultMark(false);
	//	session.SetDefaultTimestampIncrement(160);

	//	uint8_t silencebuffer[160];
	//	for (int i = 0; i < 160; i++)
	//		silencebuffer[i] = 128;

	//	RTPTime delay(0.020);
	//	RTPTime starttime = RTPTime::CurrentTime();

	//	bool done = false;

	//	//====
	//	//只有使用WindowsAPI写入文件才能同时读取播放，文件流是不行的（因为只有一个文件位置指针）
	//	HANDLE hFile = CreateFile(L"C:\\temp.mp3", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
	//		0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	//	char *FileBuf = new char[4096];
	//	ZeroMemory(FileBuf, 4096);
	//	DWORD ReadSize = 0;

	//	unsigned char *recvdata;
	//	//=============
	//	while (!done)
	//	{
	//		status = session.SendPacket(silencebuffer, 160);
	//		if (status < 0)
	//		{
	//			std::cerr << RTPGetErrorString(status) << std::endl;
	//			exit(-1);
	//		}

	//		session.BeginDataAccess();
	//		if (session.GotoFirstSource())
	//		{
	//			do
	//			{
	//				RTPPacket *packet;

	//				while ((packet = session.GetNextPacket()) != 0)
	//				{
	//					//std::cout << "Got packet with "
	//						//<< "extended sequence number "
	//						//<< packet->GetExtendedSequenceNumber()
	//						//<< " from SSRC " << packet->GetSSRC()
	//						//<< std::endl;
	//					recvdata = packet->GetPayloadData();  //取出RTP包中的数据
	//					long length;
	//					length = packet->GetPayloadLength();

	//					FileBuf = (char *)recvdata + 4;//注意去掉4字节！
	//					WriteFile(hFile, FileBuf, length - 4, &ReadSize, NULL);

	//					session.DeletePacket(packet);
	//				}
	//			} while (session.GotoNextSource());
	//		}
	//		session.EndDataAccess();

	//		RTPTime::Wait(delay);

	//		RTPTime t = RTPTime::CurrentTime();
	//		t -= starttime;
	//		if (t > RTPTime(10.0))
	//			done = true;
	//	}

	//	delay = RTPTime(10.0);
	//	session.BYEDestroy(delay, "Time's up", 9);
	//}

	return 0;
}


//RTSP控制线程
UINT rtspControl(LPVOID lpParam)
{
	rtspParam *param = (rtspParam *)lpParam;
	RTSPmsg rtspMsg;            // RTSP信令相关
	int flag;

	//======发送RTSP信令，并对答令进行相应解析
	rtspMsg.initRTSPmsg(param->URI, param->Port, param->socket);  // 初始化
	flag = rtspMsg.play();                                               // 播放，即完成从OPTION到PLAY的信令发送工作
	if (flag != 200)
	{
		cout << "Error in playing media." << endl;
		return 0;
	}

	int timer = 0;
	while (1)
	{
		timer++;           //计时器

		//心跳
		if (timer > 1000)
		{
			timer = 0;
			rtspMsg.HeartBeat();
		}





		//事件控制结束
		//WaitForSingleObject();
	}

	return 0;
}

UINT rtpHandler(LPVOID lpParam)
{
	rtpParam *rParam = (rtpParam *)lpParam;
	RTPHandler rtpHandler;

	return 0;
}
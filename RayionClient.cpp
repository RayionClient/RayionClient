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

	//�ܶ๦�ܶ�Ҫ��ʼ��Winsock�����Ե����ó�����ʼ��
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	URIInfo uri;                // URI��Ϣ���洢�ͽ���URI
	Socket sk;                  // Winsock�������1

	//=====����Ϸ����ж���URI����========
	if (argc != 4)
	{
		//������Ŀ���ԣ�����

		return 0;
	}
	
	uri.initURIInfo(argv[1]);
	//====================================

	//=======ʹ��Winsock�����������������=====
	sk.initWinsock(uri.GetDotIPAddr(), uri.GetstrPort());
	int cnct = sk.connectServer();      //�ɹ������򷵻���

	if (cnct != 0)
	{
		cout << "Failed to connect the server.";
		return 0;
	}
	//========================================

	//�����߳�
	rtspParam rtspPara;
	rtspPara.URI = argv[1];
	rtspPara.Port = uri.GetintPort();
	rtspPara.socket = sk.GetSocket();

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rtspControl, &rtspPara, 0, NULL);       //RTSP�����߳�

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


//RTSP�����߳�
UINT rtspControl(LPVOID lpParam)
{
	rtspParam *param = (rtspParam *)lpParam;
	RTSPmsg rtspMsg;            // RTSP�������
	int flag;

	//����RTSP������Դ��������Ӧ����
	rtspMsg.initRTSPmsg(param->URI, param->Port, param->RTPPort, param->socket);     // ��ʼ��
	flag = rtspMsg.play();                                           // ���ţ�����ɴ�OPTION��PLAY������͹���
	if (flag != 200)
	{
		cout << "Error in playing media." << endl;
		return 0;
	}

	// ����
	int timer = 0;
	while (1)
	{
		timer++;           //��ʱ��
		
		if (timer > 100000)
		{
			timer = 0;
			//rtspMsg.HeartBeat();
		}

		////�¼����ƽ���
		//if (WaitForSingleObject(hEvent_Close, 1) == WAIT_OBJECT_0)
		//{
		//	break;
		//}
	}

	return 0;
}

//RTP�հ��߳�
UINT rtpHandler(LPVOID lpParam)
{
	rtpParam *rParam = (rtpParam *)lpParam;
	RTPHandler rtpHandler;

	rtpHandler.initRTPHandler(rParam->IPAddr, rParam->RTPPort);
	rtpHandler.recvPackets();

	//�¼����ƽ���
	//WaitForSingleObject(hEvent_Close, INFINITE);

	return 0;
}
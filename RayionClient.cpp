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

	URIInfo uri;                // URI��Ϣ���洢�ͽ���URI
	Socket sk;                  // Winsock�������
	
	RTPHandler rtp;             // ʹ��JRTPLIB����RTP��

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
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DecoderAndPlayer, NULL, 0, NULL);


	



	//	//ʹ��jrtplib�������ݰ�
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
	//	//ֻ��ʹ��WindowsAPIд���ļ�����ͬʱ��ȡ���ţ��ļ����ǲ��еģ���Ϊֻ��һ���ļ�λ��ָ�룩
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
	//					recvdata = packet->GetPayloadData();  //ȡ��RTP���е�����
	//					long length;
	//					length = packet->GetPayloadLength();

	//					FileBuf = (char *)recvdata + 4;//ע��ȥ��4�ֽڣ�
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


//RTSP�����߳�
UINT rtspControl(LPVOID lpParam)
{
	rtspParam *param = (rtspParam *)lpParam;
	RTSPmsg rtspMsg;            // RTSP�������
	int flag;

	//======����RTSP������Դ��������Ӧ����
	rtspMsg.initRTSPmsg(param->URI, param->Port, param->socket);  // ��ʼ��
	flag = rtspMsg.play();                                               // ���ţ�����ɴ�OPTION��PLAY������͹���
	if (flag != 200)
	{
		cout << "Error in playing media." << endl;
		return 0;
	}

	int timer = 0;
	while (1)
	{
		timer++;           //��ʱ��

		//����
		if (timer > 1000)
		{
			timer = 0;
			rtspMsg.HeartBeat();
		}





		//�¼����ƽ���
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
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
	Socket sk;                  // Winsock�������
	//RTPHandler rtp;             // ʹ��JRTPLIB����RTP��

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
	rtspMsg.initRTSPmsg(param->URI, param->Port, param->socket);     // ��ʼ��
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
	//RTPHandler rtpHandler;

	/*rtpHandler.initRTPHandler(rParam->IPAddr, rParam->Port);
	rtpHandler.recvPackets();*/

	//�¼����ƽ���
	//WaitForSingleObject(hEvent_Close, INFINITE);


	RTPSession session;

	RTPSessionParams sessionparams;
	sessionparams.SetOwnTimestampUnit(1.0 / 8000.0);

	RTPUDPv4TransmissionParams transparams;
	transparams.SetPortbase(56778);

	int status = session.Create(sessionparams, &transparams);
	if (status < 0)
	{
		std::cerr << RTPGetErrorString(status) << std::endl;
		//exit(-1);
	}

	uint8_t localip[] = { 192, 168, 1, 115 };
	RTPIPv4Address addr(localip, 56778);

	status = session.AddDestination(addr);
	if (status < 0)
	{
		std::cerr << RTPGetErrorString(status) << std::endl;
		//exit(-1);
	}

	RTPTime delay(0.020);
	RTPTime starttime = RTPTime::CurrentTime();
	//==
	HANDLE hFile = CreateFile(L"C:\\rtp.tmp", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	char *FileBuf = new char[BUF_SIZE];
	ZeroMemory(FileBuf, BUF_SIZE);
	DWORD ReadSize = 0;
	unsigned char *recvdata;
	//==

	bool done = false;
	while (!done)
	{
		session.BeginDataAccess();
		if (session.GotoFirstSource())
		{
			do
			{
				RTPPacket *packet;

				while ((packet = session.GetNextPacket()) != 0)
				{
					std::cout << "Got packet with "
						<< "extended sequence number "
						<< packet->GetExtendedSequenceNumber()
						<< " from SSRC " << packet->GetSSRC()
						<< std::endl;

					recvdata = packet->GetPayloadData();  //ȡ��RTP���е�����
					long length;
					length = packet->GetPayloadLength();

					FileBuf = (char *)recvdata + 4;//ע��ȥ��4�ֽڣ�
					WriteFile(hFile, FileBuf, length - 4, &ReadSize, NULL);

					session.DeletePacket(packet);
				}
			} while (session.GotoNextSource());
		}
		session.EndDataAccess();

		RTPTime::Wait(delay);

		RTPTime t = RTPTime::CurrentTime();
		t -= starttime;
		if (t > RTPTime(60.0))
			done = true;
	}

	delay = RTPTime(10.0);
	session.BYEDestroy(delay, "Time's up", 9);



	Sleep(60000);

	return 0;
}
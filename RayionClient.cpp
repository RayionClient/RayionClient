#include "stdafx.h"
#include "URIParsing.h"

int main(int argc, char *argv[])
{
	argc = 4;
	argv[1] = "rtsp://192.168.1.115:554/live";
	argv[2] = "Reserved for login user name";
	argv[3] = "Reserved for login pwd";

	//=====����Ϸ����ж���URI����========
	if (argc != 4)
	{
		//������Ŀ���ԣ�����

		return 0;
	}

	URIInfo uri;
	uri.initURIInfo(argv[1]);
	//====================================


	//=======ʹ��Winsock�����������������=====
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// ����Socket
	SOCKET RTSPSocket = socket(AF_INET, SOCK_STREAM, 0);

	//���ӷ�����
	in_addr address;
	sockaddr_in ServerAddr;
	ServerAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "192.168.1.115", &address);
	ServerAddr.sin_addr = address;
	ServerAddr.sin_port = htons(atoi("554"));

	int cnct = connect(RTSPSocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr));//�����ǿ��ܳ�ʱ�ģ�Ĭ�ϳ�ʱʱ����75��

	if (cnct == 0)
	{
		char *msg = new char[4096];
		char *URI = "rtsp://192.168.1.115/live";
		int Seq = 1;

		ZeroMemory(msg, 4096);
		sprintf_s(msg, 4096,
			"OPTIONS %s RTSP/1.0\r\nCSeq: %d\r\n\r\n",
			URI, Seq);
		send(RTSPSocket, msg, strlen(msg), 0);
		ZeroMemory(msg, 4096);
		recv(RTSPSocket, msg, 4096, 0);

		ZeroMemory(msg, 4096);
		sprintf_s(msg, 4096,
			"DESCRIBE %s RTSP/1.0\r\nCSeq: %d\r\nAccept: application/sdp\r\n\r\n",
			URI, Seq);
		send(RTSPSocket, msg, strlen(msg), 0);
		ZeroMemory(msg, 4096);
		recv(RTSPSocket, msg, 4096, 0);
		recv(RTSPSocket, msg, 4096, 0);

		ZeroMemory(msg, 4096);
		sprintf_s(msg, 4096,
			"SETUP %s/trackID=3 RTSP/1.0\r\nCSeq: %d\r\nTransport: RTP/AVP;unicast;client_port=%s\r\n\r\n",
			URI, Seq, "554-555");
		send(RTSPSocket, msg, strlen(msg), 0);
		ZeroMemory(msg, 4096);
		recv(RTSPSocket, msg, 4096, 0);

		char *Session = new char[17];
		ZeroMemory(Session, 17);

		char *SetupHead = "Session: ";
		char *tmp = strstr(msg, SetupHead);

		for (int i = 0; i < 17; i++)
		{
			Session[i] = tmp[i + 9];
		}

		ZeroMemory(msg, 4096);
		sprintf_s(msg, 4096,
			"PLAY %s/trackID=3 RTSP/1.0\r\nCSeq: %d\r\nSession: %s\r\nRange: npt=0.000-\r\n\r\n",
			URI, Seq, Session);
		send(RTSPSocket, msg, strlen(msg), 0);
		ZeroMemory(msg, 4096);
		recv(RTSPSocket, msg, 4096, 0);

		//ʹ��jrtplib�������ݰ�
		RTPSession session;//The constructor accepts two parameter, an instance of an RTPRandom object, and an instance of an RTPMemoryManager object.

		RTPSessionParams sessionparams;
		sessionparams.SetOwnTimestampUnit(1.0 / 8000.0);

		RTPUDPv4TransmissionParams transparams;
		transparams.SetPortbase(8000);

		int status = session.Create(sessionparams, &transparams);
		if (status < 0)
		{
			std::cerr << RTPGetErrorString(status) << std::endl;
			exit(-1);
		}

		uint8_t localip[] = { 127, 0, 0, 1 };
		RTPIPv4Address addr(localip, 80);

		status = session.AddDestination(addr);
		if (status < 0)
		{
			std::cerr << RTPGetErrorString(status) << std::endl;
			exit(-1);
		}

		session.SetDefaultPayloadType(96);
		session.SetDefaultMark(false);
		session.SetDefaultTimestampIncrement(160);

		uint8_t silencebuffer[160];
		for (int i = 0; i < 160; i++)
			silencebuffer[i] = 128;

		RTPTime delay(0.020);
		RTPTime starttime = RTPTime::CurrentTime();

		bool done = false;

		//====
		//ֻ��ʹ��WindowsAPIд���ļ�����ͬʱ��ȡ���ţ��ļ����ǲ��еģ���Ϊֻ��һ���ļ�λ��ָ�룩
		HANDLE hFile = CreateFile(L"C:\\temp.mp3", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
			0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		char *FileBuf = new char[4096];
		ZeroMemory(FileBuf, 4096);
		DWORD ReadSize = 0;

		unsigned char *recvdata;
		//=============
		while (!done)
		{
			status = session.SendPacket(silencebuffer, 160);
			if (status < 0)
			{
				std::cerr << RTPGetErrorString(status) << std::endl;
				exit(-1);
			}

			session.BeginDataAccess();
			if (session.GotoFirstSource())
			{
				do
				{
					RTPPacket *packet;

					while ((packet = session.GetNextPacket()) != 0)
					{
						//std::cout << "Got packet with "
							//<< "extended sequence number "
							//<< packet->GetExtendedSequenceNumber()
							//<< " from SSRC " << packet->GetSSRC()
							//<< std::endl;
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
			if (t > RTPTime(10.0))
				done = true;
		}

		delay = RTPTime(10.0);
		session.BYEDestroy(delay, "Time's up", 9);
	}

	//һ��������
	shutdown(RTSPSocket, SD_BOTH);
	closesocket(RTSPSocket);
	WSACleanup();

	return 0;
}


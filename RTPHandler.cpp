#include "stdafx.h"
#include "RTPHandler.h"

int RTPHandler::initRTPHandler(int *address, int rtpPort)
{
	IPAddr = address;
	Port = rtpPort;

	//Create Session
	sessionparams.SetOwnTimestampUnit(1.0 / 44100.0);
	transparams.SetPortbase(Port);
	session.Create(sessionparams, &transparams);

	return 0;
}

int RTPHandler::recvPackets()
{
	int status;

	RTPIPv4Address addr(*IPAddr, 554);
	status = session.AddDestination(addr);

	//WaitForSingleObject(RTSP.hEvent_RTP, INFINITE);

	unsigned char *recvdata;

	RTPTime delay(0.000);
	RTPTime starttime = RTPTime::CurrentTime();
	bool done = false;

	//File buffer
	rtpBuffer.setBufPath("C:/rtp.tmp");
	rtpBuffer.CreateBuffer();

	long length;

	while (!done)
	{
		session.BeginDataAccess();
		if (session.GotoFirstSourceWithData())
		{
			do
			{
				RTPPacket *packet;
				while ((packet = session.GetNextPacket()) != 0)
				{
					recvdata = packet->GetPayloadData();                    // ȡ��RTP���е��غ�����
					length = packet->GetPayloadLength();                    // ��ȡ�غɳ���

					rtpBuffer.tmpbuf = (char *)recvdata + 4;                // (ע��ȥ��4�ֽڣ�)�Ǹ�����mp3�Ĵ���
					rtpBuffer.WriteBuffer(rtpBuffer.tmpbuf, length);        // ����ʱ�����е�����д���ļ�
					
					session.DeletePacket(packet);
				}
			} while (session.GotoNextSourceWithData());
		}
		session.EndDataAccess();

		RTPTime t = RTPTime::CurrentTime();
		t -= starttime;//CurrentTime - StartTime�����Ѿ���ȥ��ʱ��

		//��ʱ��������ˣ��¼����ƽ�������

		//if (WaitForSingleObject(RTSP.hEvent_Close, 1) == WAIT_OBJECT_0)
		//{
		//	//������ֹͣ���û�ֹͣ�˲���
		//	break;
		//}

	}
	delay = RTPTime(10.0);
	session.BYEDestroy(delay, "Time's up", 9);

	return 0;
}

int RTPBuffer::setBufPath(char *path)
{
	bufFilePath = path;

	return 0;
}

int RTPBuffer::CreateBuffer()
{
	//ֻ��ʹ��WindowsAPIд���ļ�����ͬʱ��ȡ���ţ��ļ����ǲ��еģ���Ϊֻ��һ���ļ�λ��ָ�룩
	hFile = CreateFile((LPCWSTR)bufFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	return 0;
}

int RTPBuffer::WriteBuffer(char *tmp, long length)
{
	DWORD ReadSize;

	WriteFile(hFile, tmp, length - 4, &ReadSize, NULL);

	return 0;
}
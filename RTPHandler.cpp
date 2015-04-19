#include "stdafx.h"
#include "RTPHandler.h"

//��ʼ��
int RTPHandler::initRTPHandler(int *address, int rtpPort)
{
	IPAddr = address;
	Port = rtpPort;

	//Create Session
	sessionparams.SetOwnTimestampUnit(1.0 / 8000.0);
	transparams.SetPortbase(Port);
	session.Create(sessionparams, &transparams);

	return 0;
}

//�հ�
int RTPHandler::recvPackets()
{
	int status;

	uint8_t localip[] = { IPAddr[0], IPAddr[1], IPAddr[2], IPAddr[3] };
	RTPIPv4Address addr(localip, Port);
	status = session.AddDestination(addr);

	//�ȴ�����
	//WaitForSingleObject(hEvent_RTP, INFINITE);

	unsigned char *recvdata;

	RTPTime delay(0.020);
	RTPTime starttime = RTPTime::CurrentTime();
	bool done = false;

	//File buffer
	//rtpBuffer.setBufPath("C:\\rtp.tmp");
	//rtpBuffer.CreateBuffer();

	wchar_t *tmp = L"C:\\rtp.tmp";
	HANDLE hFile = CreateFile(tmp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	char *FileBuf = new char[BUF_SIZE];
	ZeroMemory(FileBuf, BUF_SIZE);
	DWORD ReadSize = 0;





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
					/*cout << "Got packet with "
						<< "extended sequence number "
						<< packet->GetExtendedSequenceNumber()
						<< " from SSRC " << packet->GetSSRC()
						<< endl;*/

					recvdata = packet->GetPayloadData();                    // ȡ��RTP���е��غ�����
					length = packet->GetPayloadLength();                    // ��ȡ�غɳ���
					//cout << length << endl;

					//rtpBuffer.tmpbuf = (char *)recvdata + 4;                // (ע��ȥ��4�ֽڣ�)�Ǹ�����mp3�Ĵ���
					//rtpBuffer.WriteBuffer(rtpBuffer.tmpbuf, length);        // ����ʱ�����е�����д���ļ�

					FileBuf = (char *)recvdata + 4;//ע��ȥ��4�ֽڣ�
					WriteFile(hFile, FileBuf, length - 4, &ReadSize, NULL);


					
					session.DeletePacket(packet);
				}
			} while (session.GotoNextSourceWithData());
		}
		session.EndDataAccess();

		//��ʱ��
		RTPTime t = RTPTime::CurrentTime();
		t -= starttime;//CurrentTime - StartTime�����Ѿ���ȥ��ʱ��

		//��ʱ��������ˣ��¼����ƽ�������

		//if (WaitForSingleObject(hEvent_Close, 1) == WAIT_OBJECT_0)
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
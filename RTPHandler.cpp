#include "stdafx.h"
#include "RTPHandler.h"

//初始化
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

//收包
int RTPHandler::recvPackets()
{
	int status;

	uint8_t localip[] = { IPAddr[0], IPAddr[1], IPAddr[2], IPAddr[3] };
	RTPIPv4Address addr(localip, Port);
	status = session.AddDestination(addr);

	//等待启动
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

					recvdata = packet->GetPayloadData();                    // 取出RTP包中的载荷数据
					length = packet->GetPayloadLength();                    // 获取载荷长度
					//cout << length << endl;

					//rtpBuffer.tmpbuf = (char *)recvdata + 4;                // (注意去掉4字节！)那个是收mp3的处理
					//rtpBuffer.WriteBuffer(rtpBuffer.tmpbuf, length);        // 把临时缓存中的数据写入文件

					FileBuf = (char *)recvdata + 4;//注意去掉4字节！
					WriteFile(hFile, FileBuf, length - 4, &ReadSize, NULL);


					
					session.DeletePacket(packet);
				}
			} while (session.GotoNextSourceWithData());
		}
		session.EndDataAccess();

		//计时器
		RTPTime t = RTPTime::CurrentTime();
		t -= starttime;//CurrentTime - StartTime，即已经逝去的时间

		//超时这个就免了，事件控制结束就行

		//if (WaitForSingleObject(hEvent_Close, 1) == WAIT_OBJECT_0)
		//{
		//	//非正常停止：用户停止了播放
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
	//只有使用WindowsAPI写入文件才能同时读取播放，文件流是不行的（因为只有一个文件位置指针）
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
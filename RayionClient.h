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

// �߳�
UINT rtspControl(LPVOID lpParam);          // һ��RTSP�����̣߳������������
UINT rtpHandler(LPVOID lpParam);            // RTP���հ�������װ�뻺���߳�
UINT DecoderAndPlayer(LPVOID lpParam);        // �����벥���߳�

//�̲߳�����
class rtspParam
{
public:
	char *URI;
	int Port;
	int RTPPort;
	SOCKET socket;

	rtspParam()
	{
		URI = new char[BUF_SIZE];
		Port = 554;
		socket = -1;
		RTPPort = RTP_PORT;
	}

	~rtspParam(){};
};

class rtpParam
{
public:
	int *IPAddr;
	int Port;
	int RTPPort;

	rtpParam()
	{
		IPAddr = new int[4];
		Port = 554;
		RTPPort = RTP_PORT;
	}

	~rtpParam(){};
};
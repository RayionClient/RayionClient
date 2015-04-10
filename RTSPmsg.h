//RTSP�����
#pragma once

class RTSPmsg
{
private:
	int errCode;                  // �����루200ΪOK��
	char *URI;                    // URI��ַ
	int rtspPort;               // RTSP�˿ںţ���ʵ������û��ϵ�����Ӧ�ö���ָ����������ż��
	SOCKET rtspSocket;            // ����������Ϣ��Socket
	int Seq;                      // ��Ϣ���
	char *Session;                // �ỰID
	char *track;                  // ���ID

	int sendMsg(int message);      // ����RTSP����
	int parseReply(int message);   // ����RTSP����
	//int errMsg(int errCode);
	//char *getMsgName(int message);

public:
	RTSPmsg();
	~RTSPmsg(){};

	int initRTSPmsg(char *uri, int Port, SOCKET Socket);  // ��ʼ��
	int play();                              // ����ָ��
	//int HeartBeat();                         // ������ά��RTSP����
	//int stop();
};
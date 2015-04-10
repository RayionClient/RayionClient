//RTSP信令处理
#pragma once

class RTSPmsg
{
private:
	int errCode;                  // 错误码（200为OK）
	char *URI;                    // URI地址
	int rtspPort;               // RTSP端口号，其实和连接没关系，这个应该独立指定但必须是偶数
	SOCKET rtspSocket;            // 用来发送信息的Socket
	int Seq;                      // 信息序号
	char *Session;                // 会话ID
	char *track;                  // 轨道ID

	int sendMsg(int message);      // 发送RTSP信令
	int parseReply(int message);   // 接收RTSP信令
	//int errMsg(int errCode);
	//char *getMsgName(int message);

public:
	RTSPmsg();
	~RTSPmsg(){};

	int initRTSPmsg(char *uri, int Port, SOCKET Socket);  // 初始化
	int play();                              // 播放指令
	//int HeartBeat();                         // 心跳，维持RTSP连接
	//int stop();
};
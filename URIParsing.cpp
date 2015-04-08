#include "stdafx.h"
#include "URIParsing.h"

//构造函数
URIInfo::URIInfo()
{
	dotIPAddr = new char[18];
	ZeroMemory(dotIPAddr, 18);
	IPAddr = new int[4];

	strPort = new char[6];
	ZeroMemory(strPort, 6);
}

//初始化URI信息
int URIInfo::initURIInfo(char *inputURI)
{
	int flag = -1;

	URI = inputURI;

	flag = ParseURI();

	//返回解析结果（是否成功）
	return flag;
}

//解析URI
int URIInfo::ParseURI()
{
	int flag = -1;

	char *substr = new char[256];
	ZeroMemory(substr, 256);

	int length = 0;
	
	//去掉RTSP头部（其实就是提取出"rtsp://..."字段）
	char *head = "rtsp://";
	substr = strstr(URI, head);

	if (substr == NULL)
	{
		cout << "Invalid Input";
		return -1;
	}

	length = strlen(substr);

	//7是"rtsp://"的长度
	for (int i = 7; i < length; i++)
	{
		//查找冒号，冒号前面的就是点分式IP了
		if (substr[i] == ':')
		{
			//提取出点分式IP地址或域名
			for (int j = 0; j < i - 7; j++)
			{
				dotIPAddr[j] = substr[7 + j];
			}
			
			//解析域名
			//暂时不写，先用IP地址测试流程

			//点分式IP地址转成十进制IP地址
			DWORD iplong;
			inet_pton(AF_INET, dotIPAddr, &iplong);

			for (int j = 0; j < 4; j++)
			{
				IPAddr[j] = iplong >> (j * 8) & 255;
			}

			//提取出端口号
			for (int j = i + 1; j < length; j++)
			{
				if (substr[j] == '/')break;
				strPort[j - i - 1] = substr[j];
			}
			//字符端口号转十进制
			intPort = atoi(strPort);

			//jrtplib只接受偶数端口号，奇数则报错
			if (intPort % 2 != 0)
			{
				cout << "Non-even port number.";
				return -1;
			}
		}
	}
	
	return flag;
}

// 获取点分式IPv4地址
char* URIInfo::GetDotIPAddr()
{
	return dotIPAddr;
}

// 获取十进制IPv4地址
int* URIInfo::GetIPAddr()
{
	return IPAddr;
}

// 获取字符形式端口号
char* URIInfo::GetstrPort()
{
	return strPort;
}

// 获取十进制端口号
int URIInfo::GetintPort()
{
	return intPort;
}
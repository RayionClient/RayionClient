#include "stdafx.h"
#include "URIParsing.h"

//���캯��
URIInfo::URIInfo()
{
	dotIPAddr = new char[18];
	ZeroMemory(dotIPAddr, 18);
	IPAddr = new int[4];

	strPort = new char[6];
	ZeroMemory(strPort, 6);
}

//��ʼ��URI��Ϣ
int URIInfo::initURIInfo(char *inputURI)
{
	int flag = -1;

	URI = inputURI;

	flag = ParseURI();

	//���ؽ���������Ƿ�ɹ���
	return flag;
}

//����URI
int URIInfo::ParseURI()
{
	int flag = -1;

	char *substr = new char[256];
	ZeroMemory(substr, 256);

	int length = 0;
	
	//ȥ��RTSPͷ������ʵ������ȡ��"rtsp://..."�ֶΣ�
	char *head = "rtsp://";
	substr = strstr(URI, head);

	if (substr == NULL)
	{
		cout << "Invalid Input";
		return -1;
	}

	length = strlen(substr);

	//7��"rtsp://"�ĳ���
	for (int i = 7; i < length; i++)
	{
		//����ð�ţ�ð��ǰ��ľ��ǵ��ʽIP��
		if (substr[i] == ':')
		{
			//��ȡ�����ʽIP��ַ������
			for (int j = 0; j < i - 7; j++)
			{
				dotIPAddr[j] = substr[7 + j];
			}
			
			//��������
			//��ʱ��д������IP��ַ��������

			//���ʽIP��ַת��ʮ����IP��ַ
			DWORD iplong;
			inet_pton(AF_INET, dotIPAddr, &iplong);

			for (int j = 0; j < 4; j++)
			{
				IPAddr[j] = iplong >> (j * 8) & 255;
			}

			//��ȡ���˿ں�
			for (int j = i + 1; j < length; j++)
			{
				if (substr[j] == '/')break;
				strPort[j - i - 1] = substr[j];
			}
			//�ַ��˿ں�תʮ����
			intPort = atoi(strPort);

			//jrtplibֻ����ż���˿ںţ������򱨴�
			if (intPort % 2 != 0)
			{
				cout << "Non-even port number.";
				return -1;
			}
		}
	}
	
	return flag;
}

// ��ȡ���ʽIPv4��ַ
char* URIInfo::GetDotIPAddr()
{
	return dotIPAddr;
}

// ��ȡʮ����IPv4��ַ
int* URIInfo::GetIPAddr()
{
	return IPAddr;
}

// ��ȡ�ַ���ʽ�˿ں�
char* URIInfo::GetstrPort()
{
	return strPort;
}

// ��ȡʮ���ƶ˿ں�
int URIInfo::GetintPort()
{
	return intPort;
}
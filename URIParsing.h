//URI������
#pragma once

class URIInfo
{
private:
	char *URI;             // URI��ַ
	char *dotIPAddr;       // ���ʽIPv4��ַ
	int *IPAddr;           // ʮ����IPv4��ַ
	char *strPort;         // �ַ���ʽ�˿ں�
	int intPort;           // ʮ���ƶ˿ں�
	
	int ParseURI();        // ��������

public:
	URIInfo();
	~URIInfo(){};

	int initURIInfo(char *inputURI);    // ��ʼ���������򷵻�-1���ɹ��򷵻�0;

	char GetDotIPAddr();                // ��ȡ���ʽIPv4��ַ
	int GetIPAddr();                    // ��ȡʮ����IPv4��ַ
	char GetstrPort();                  // ��ȡ�ַ���ʽ�˿ں�
	int GetintPort();                   // ��ȡʮ���ƶ˿ں�
};
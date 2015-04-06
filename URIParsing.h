//URI解析器
#pragma once

class URIInfo
{
private:
	char *URI;             // URI地址
	char *dotIPAddr;       // 点分式IPv4地址
	int *IPAddr;           // 十进制IPv4地址
	char *strPort;         // 字符形式端口号
	int intPort;           // 十进制端口号
	
	int ParseURI();        // 解析函数

public:
	URIInfo();
	~URIInfo(){};

	int initURIInfo(char *inputURI);    // 初始化，错误则返回-1，成功则返回0;

	char GetDotIPAddr();                // 获取点分式IPv4地址
	int GetIPAddr();                    // 获取十进制IPv4地址
	char GetstrPort();                  // 获取字符形式端口号
	int GetintPort();                   // 获取十进制端口号
};
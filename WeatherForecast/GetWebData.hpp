#pragma once
#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using std::string;
class GetWebData {
	public:
		static int downloadHTML();

	private:	
		HINSTANCE hInst;
		WSADATA wsaData;
		static void mParseUrl(char *mUrl, string &serverName, string &filepath, string &filename);
		static SOCKET connectToServer(char *szServerName, WORD portNum);
		static int getHeaderLength(char *content);
		static char *readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut);

};
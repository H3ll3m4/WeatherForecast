#include "stdafx.h"
//https://stackoverflow.com/questions/4726155/what-is-stdafx-h-used-for-in-visual-studio/4726838#4726838
# include "GetWebData.hpp"

using std::cout;
using std::cin;
using std::endl;

int GetWebData::downloadHTML()
{
	WSADATA wsaData;
	const int bufLen = 1024;
	char *szUrl = (char *) "https://www.meteociel.fr/temps-reel/obs_villes.php?code2=82";
	long fileSize;
	char *memBuffer, *headerBuffer;
	FILE *fp;
	memBuffer = headerBuffer = NULL;
	if (WSAStartup(0x101, &wsaData) != 0)
		return -1;
	memBuffer = readUrl2(szUrl, fileSize, &headerBuffer);
	//printf("returned from readUrl\n");
	//printf("data returned:\n%s", memBuffer);
	if (fileSize != 0)
	{
		//printf("Got some data\n");
		fp = fopen("downloaded.html", "wb");
		//fp = fopen_s("downloaded.html", "wb");
		fwrite(memBuffer, 1, fileSize, fp);
		fclose(fp);
		delete(memBuffer);
		delete(headerBuffer);
	}
	WSACleanup();
	return 0;
}

//https://c.developpez.com/cours/sockets-c-cpp-demystifies/
int socketCreation() {
	WSADATA WSAData;
	SOCKET sock;
	SOCKADDR_IN sin;
	char buffer[10000];
	WSAStartup(MAKEWORD(2, 0), &WSAData);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	struct hostent *host;
	string url = "https://www.meteociel.fr/temps-reel/obs_villes.php?code2=82";
	host = gethostbyname(url.c_str());
	sin.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(6667);
	if (connect(sock, (SOCKADDR *)&sin, sizeof(sin)) != 0)
	{
		//cout << "Could not connect";
		system("pause");
		return 1;
	}
	string get_http = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";
	if (send(sock, get_http.c_str(), strlen(get_http.c_str()), 0) > 0) {
		//cout << "Could not send anything";
		system("pause");
		return 1;
	}
	if (recv(sock, buffer, sizeof(buffer), 0) < 0)
	{
		//cout << "Could not receive anything";
		system("pause");
		return 1;
	}
	//cout << "We received:" << buffer << endl;
	closesocket(sock);
	WSACleanup();
	return 0;
}

//https://stackoverflow.com/questions/1011339/how-do-you-make-a-http-request-with-c/51959694#51959694
void GetWebData::mParseUrl(char *mUrl, string &serverName, string &filepath, string &filename)
{
	string::size_type n;
	string url = mUrl;
	if (url.substr(0, 7) == "http://")
		url.erase(0, 7);

	if (url.substr(0, 8) == "https://")
		url.erase(0, 8);
	n = url.find('/');
	if (n != string::npos)
	{
		serverName = url.substr(0, n);
		filepath = url.substr(n);
		n = filepath.rfind('/');
		filename = filepath.substr(n + 1);
	}
	else
	{
		serverName = url;
		filepath = "/";
		filename = "";
	}
}

SOCKET GetWebData::connectToServer(char *szServerName, WORD portNum)
{
	struct hostent *hp;
	unsigned int addr;
	struct sockaddr_in server;
	SOCKET conn;
	conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (conn == INVALID_SOCKET)
		return NULL;

	if (inet_addr(szServerName) == INADDR_NONE)
	{
		hp = gethostbyname(szServerName);
	}
	else
	{
		addr = inet_addr(szServerName);
		hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	}
	if (hp == NULL)
	{
		closesocket(conn);
		return NULL;
	}
	server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(portNum);
	if (connect(conn, (struct sockaddr*)&server, sizeof(server)))
	{
		closesocket(conn);
		return NULL;
	}
	return conn;
}

int GetWebData::getHeaderLength(char *content)
{
	const char *srchStr1 = "\r\n\r\n", *srchStr2 = "\n\r\n\r";
	char *findPos;
	int ofset = -1;
	findPos = strstr(content, srchStr1);
	if (findPos != NULL)
	{
		ofset = findPos - content;
		ofset += strlen(srchStr1);
	}
	else
	{
		findPos = strstr(content, srchStr2);
		if (findPos != NULL)
		{
			ofset = findPos - content;
			ofset += strlen(srchStr2);
		}
	}
	return ofset;
}

char *GetWebData::readUrl2(char *szUrl, long &bytesReturnedOut, char **headerOut)
{
	const int bufSize = 512;
	char readBuffer[bufSize], sendBuffer[bufSize], tmpBuffer[bufSize];
	char *tmpResult = NULL, *result;
	SOCKET conn;
	string server, filepath, filename;
	long totalBytesRead, thisReadSize, headerLen;

	mParseUrl(szUrl, server, filepath, filename);

	///////////// step 1, connect //////////////////////
	conn = connectToServer((char*)server.c_str(), 80);

	///////////// step 2, send GET request /////////////
	sprintf(tmpBuffer, "GET %s HTTP/1.0", filepath.c_str());
	strcpy(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	sprintf(tmpBuffer, "Host: %s", server.c_str());
	strcat(sendBuffer, tmpBuffer);
	strcat(sendBuffer, "\r\n");
	strcat(sendBuffer, "\r\n");
	send(conn, sendBuffer, strlen(sendBuffer), 0);

	//    SetWindowText(edit3Hwnd, sendBuffer);
	//printf("Buffer being sent:\n%s", sendBuffer);

	///////////// step 3 - get received bytes ////////////////
	// Receive until the peer closes the connection
	totalBytesRead = 0;
	while (1)
	{
		memset(readBuffer, 0, bufSize);
		thisReadSize = recv(conn, readBuffer, bufSize, 0);

		if (thisReadSize <= 0)
			break;

		tmpResult = (char*)realloc(tmpResult, thisReadSize + totalBytesRead);

		memcpy(tmpResult + totalBytesRead, readBuffer, thisReadSize);
		totalBytesRead += thisReadSize;
	}

	headerLen = getHeaderLength(tmpResult);
	long contenLen = totalBytesRead - headerLen;
	result = new char[contenLen + 1];
	memcpy(result, tmpResult + headerLen, contenLen);
	result[contenLen] = 0x0;
	char *myTmp;

	myTmp = new char[headerLen + 1];
	strncpy(myTmp, tmpResult, headerLen);
	myTmp[headerLen] = NULL;
	delete(tmpResult);
	*headerOut = myTmp;

	bytesReturnedOut = contenLen;
	closesocket(conn);
	return(result);
}
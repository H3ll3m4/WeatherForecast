// WeatherForecast.cpp : Defines the entry point for the console application.
//
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
#include <iostream>
#include <fstream>
#include "WeatherData.h"
#include "GetWebData.hpp"
#pragma comment(lib, "ws2_32.lib")
int main()
{
	//connect to API (find an API that archive data)
	//https://www.meteociel.fr/temps-reel/obs_villes.php?code2=82

	GetWebData::downloadHTML();
	WeatherData::extractData();

	//Kepp C++ console application open
	std::getchar();

	return 0;
}
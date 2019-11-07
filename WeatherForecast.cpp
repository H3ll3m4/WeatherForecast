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
	//http://lpo.dt.navy.mil/data/DM/
	//https://www.meteociel.fr/temps-reel/obs_villes.php?code2=82

	//socketCreation1();
	//socketCreation3();
	GetWebData::downloadHTML();

	//Look for "<H1><center> Tableau d'observations pour <br>"
	WeatherData::extractData();
	//std::cout << WeatherData::nbHoursElapsedToday() << std::endl;

	//Save to DB with SQL Lite
	//WeatherData.storeDB();
	
	
	//Create a server that provides CGI service
	//Do a SQL query to get the mean of the barometric pressure and other data


	//Kepp C++ console application open
	std::getchar();

	return 0;
}
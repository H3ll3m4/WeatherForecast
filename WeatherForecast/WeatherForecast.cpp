// WeatherForecast.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "SimpleCGI.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
int main()
{
	//connect to API (find an API that archive data)
	//https://www.meteociel.fr/temps-reel/obs_villes.php?code2=82

	//GetWebData::downloadHTML();

	//Look for "<H1><center> Tableau d'observations pour <br>"
	//WeatherData::extractData();
	//std::cout << WeatherData::nbHoursElapsedToday() << std::endl;

	//Save to DB with SQL Lite
	//WeatherData.storeDB();
	//WeatherData wd = WeatherData::WeatherData();
	
	//Create a server that provides CGI service
	//Do a SQL query to get the mean of the barometric pressure and other data
	SimpleCGI cgi;
	cgi.executeQuery();

	//Kepp C++ console application open
	std::getchar();
	//system("pause");

	return 0;
}
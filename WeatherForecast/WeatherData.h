//#include "stdafx.h"
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <ratio>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>
#include <algorithm>
//#include <boost/regex.hpp>
#include <stdarg.h>
#include "DataStorage.h"
#include "GetWebData.hpp"


class WeatherData {
public:
	WeatherData();
	WeatherData(std::string date);
	std::string _db_name;
	std::vector<float> _arrTemp, _arrPress, _arrRain;
	int extractData();
	float getAvg(std::vector<float> vector);
	std::string simpleWeatherForecast(std::vector<float> vector);
	void ShowStats();
	std::string returnStats();
	std::string returnJSON();
	void makeJSON(float temp, float press, float rain, std::string forecast);
	int newfmt(char** str, const char *format, ...);
	void makeJSONArchive(std::string date, float temp, float press, float rain);

private:
	const char * _JSON_FORMAT_TODAY;
	const char *_JSON_FORMAT_ARCHIVE;
	std::string _statJSON;
	DataStorage initialiseData();
	int nbHoursElapsedToday();
};
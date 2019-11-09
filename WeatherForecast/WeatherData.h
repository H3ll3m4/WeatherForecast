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

class WeatherData {
public:
	WeatherData();
	std::string _db_name;
	int extractData();
	float getAvg(std::vector<float> vector);
	void simpleWeatherForecast(std::vector<float> vector);
	void ShowStats();
private:
	std::vector<float> _arrTemp, _arrPress, _arrRain;
	int nbHoursElapsedToday();
};
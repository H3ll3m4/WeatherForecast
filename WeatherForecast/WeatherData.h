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
#include <vector>
#include <algorithm>

class WeatherData {
public:

	static int extractData();
	static float getAvg(std::vector<float> vector);
	static void simpleWeatherForecast(std::vector<float> vector);

private:
	static int nbHoursElapsedToday();
};
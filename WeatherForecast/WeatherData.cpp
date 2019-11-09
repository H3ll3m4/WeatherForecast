#include "stdafx.h"
//https://stackoverflow.com/questions/4726155/what-is-stdafx-h-used-for-in-visual-studio/4726838#4726838
#include "WeatherData.h"
#include "DataStorage.h"
#include "GetWebData.hpp"

WeatherData::WeatherData() {
	std::cout << "Weather Data Constructor" << std::endl;
	//Check if data is available
	//Donwload data
	GetWebData::downloadHTML();
	//extractData
	WeatherData::extractData();
	ShowStats();
	//Build the DB
	//DataStorage::storeToDB(_arrTemp,_arrPress, _arrRain);
	DataStorage st = DataStorage();
	st.storeToDB(_arrTemp, _arrPress, _arrRain);
	//Query the DB
	st.showStatistics();
	//st.SelectThisYear();
}

float WeatherData::getAvg(std::vector<float> vector) {
	float res = 0;
	for (float val : vector) {
		res += val;
	}
	res /= vector.size();
	return res;
}

void WeatherData::simpleWeatherForecast(std::vector<float> pressures) {
	float slope;
	if (pressures.size() > 2) {
		//Slope calculation
		if (pressures.size() < 4) {
			slope = pressures.end() - pressures.begin();
		}
		else {
			slope = pressures[pressures.size()-1] - pressures[pressures.size() - 5];
		}
		//Analysis
		if (abs(slope) < 1) {//change is negligeable
			std::cout << "The weather is stable" << '\n';
		}
		else {
			if (slope < 0) {
				std::cout << "Weather won't be clement" << '\n';
			}
			else {
				std::cout << "Weather will turn clear and sunny" << '\n';
			}
		}
	}
	else {
		std::cout << "More data is needed" << std::endl;
	}
}

void WeatherData::ShowStats() {
	//Get the statistics:
	std::cout << "The average barometric pressure today is:" << getAvg(_arrPress) << '\n';
	std::cout << "The average temperature today is:" << getAvg(_arrTemp) << '\n';
	std::cout << "The average rain rate today is:" << getAvg(_arrRain) << '\n';
	std::cout << '\n';
	simpleWeatherForecast(_arrPress);
}

int WeatherData::extractData() {
	std::ifstream fp("downloaded.html");
	std::string strToFind = "<H1><center> Tableau d'observations pour <br>";
	std::string str;
	while (std::getline(fp, str)) {
		if (str.find(strToFind) != std::string::npos) {
			std::cout << "Table with sensor measurements is found!" << '\n';
			break;
		}
	}
	std::getline(fp,str);

	//tested on https://regexr.com/
	//Define the regex to extract temperature, pressure and rain rate
	std::regex rgxTemp("[0-9]{1,2}(\.\d)? °C");

	std::regex rgxPress("[0-9]{3,4}(\\.\\d)? hPa");

	std::regex rgxRain("[0-9]{1,2}(\.\d){0,1} mm/h");
	//Calculate the number of results, depending of the current time:
	const int nb_mm = nbHoursElapsedToday();

	//https://stackoverflow.com/questions/16749069/c-split-string-by-regex/16752826
	//Using regex_search: http://www.cplusplus.com/reference/regex/regex_search/
	//printf("The line str = %s", str);
	//std::cout << str << std::endl;
	std::smatch m,m2,m3;

	//std::vector<float> arrTemp, arrRain, arrPress;
	for (int i = 0; i < nb_mm ; i++) {
		std::regex_search(str, m, rgxTemp);
		if (m.size() > 0) {
			std::string first = m[0];
			std::string v = first.substr(0, first.find(" °C"));
			try {
				float val = stof(v);
				_arrTemp.push_back(val);
			}
			catch (int e) {
				std::cout << "Cannot convert to float" << std::endl;
				std::cout << "An exception occurred. Exception Nr. " << e << '\n';
			}
		}
		else {
			std::cout << "Not match was found for temperature regex" << std::endl;
		}

		//Pressures
		std::regex_search(str, m2, rgxPress);
		if (m2.size() > 0) {
			std::string first2 = m2[0];
			std::string v2 = first2.substr(0, first2.find(" hPa"));
			try {
				float val2 = stof(v2); 
				_arrPress.push_back(val2);
			}
			catch (int e) {
				std::cout << "Cannot convert to number" << std::endl;
				std::cout << "An exception occurred. Exception Nr. " << e << '\n';
			}
		}
		else {
			std::cout << "Not match was found for barometric pressure regex" << std::endl;
		}

		//Rain Rate
		std::regex_search(str, m3, rgxRain);
		if (m3.size() > 0) {
			std::string first3 = m3[0];
			std::string v3 = first3.substr(0, first3.find(" mm/h"));
			try {
				float val3 = stof(v3); 
				_arrRain.push_back(val3);
			}
			catch (int e) {
				std::cout << "Cannot convert to float" << std::endl;
				std::cout << "An exception occurred. Exception Nr. " << e << '\n';
			}
		}
		else {
			std::cout << "Not match was found for barometric pressure regex" << std::endl;
		}
		//next line
		std::getline(fp, str);
	}

	//std::cout << "Display content of arrTemp " << std::endl;
	//for (int i = 0; i<arrTemp.size(); i++)
	//{
	//	printf("\r %2.1f \n", arrTemp[i]);
	//}
	//std::cout << "\n" << std::endl;

	//std::cout << "Display content of arrRain " << std::endl;
	//for (int i = 0; i<arrRain.size(); i++)
	//{
	//	printf("\r %2.1f \n", arrRain[i]);
	//}
	//std::cout << "\n" << std::endl;

	//std::cout << "Display content of arrPress " << std::endl;
	//for (int i = 0; i<arrPress.size(); i++)
	//{
	//	printf("\r %4.1f \n", arrPress[i]);
	//}
	//std::cout << "\n" << std::endl;

	return 0;
}

int WeatherData::nbHoursElapsedToday(){
	using namespace std::chrono;
	//Get the time
	//auto now = std::chrono::system_clock::now();
	//time_t tnow = std::chrono::system_clock::to_time_t(now);
	//tm *date = std::localtime(&tnow);
	//int hours = date->tm_hour;
	//UTC Time:
	//https://en.cppreference.com/w/cpp/chrono/c/gmtime
	tm *tptr;
	std::time_t t = std::time(nullptr);
	tptr = std::gmtime(&t);
	//Extract the hour
	int hours = tptr->tm_hour;
	//Add 1 because it is french weather and Paris is UTC + 1
	return hours+1;
}










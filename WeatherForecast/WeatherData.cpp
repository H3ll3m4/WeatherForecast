#include "stdafx.h"
//https://stackoverflow.com/questions/4726155/what-is-stdafx-h-used-for-in-visual-studio/4726838#4726838
#include "WeatherData.h"


WeatherData::WeatherData() {
	initialiseData();
}

WeatherData::WeatherData(std::string date) {
	DataStorage st = initialiseData();
	if (!date.empty()) {
		makeJSONArchive(date, st.getAvgTemperature(date), st.getAvgPressure(date), st.getAvgRainRate(date));
	}
	else {
		//std::cerr << "Date wasn't read properly" << '\n';
	}
		
}

DataStorage WeatherData::initialiseData() {
	_JSON_FORMAT_TODAY =
		"{ \"%s\": \n  {\n"
		"    \"temperatureavg\": %lf,\n"
		"    \"barometricpressureavg\": %lf,\n"
		"    \"rainrateavg\": %lf,\n"
		"    \"forecast\": %s\n"
		"  } \n}\n"
		;
	_JSON_FORMAT_ARCHIVE =
		"{ \"%s\": \n  {\n"
		"    \"temperatureAvg\": %lf,\n"
		"    \"barometricPressureAvg\": %lf,\n"
		"    \"rainRateAvg\": %lf\n"
		"  } \n}\n"
		;
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
	//st.showStatistics();
	//st.SelectThisYear();
	return st;
}

//https://www.tutorialspoint.com/c_standard_library/c_function_vsprintf.htm
int WeatherData::newfmt(char** str, const char *format, ...) {
	va_list aptr;
	int ret;
	va_start(aptr, format);
	int len = _vscprintf(format, aptr) + 1;
	*str = (char *)malloc(len * sizeof(char));
	if (!*str) return 0;
	ret = vsprintf_s(*str, len, format, aptr);
	va_end(aptr);
	return(ret);
}

//For today data
void WeatherData::makeJSON(float temp, float press, float rain, std::string forecast) {
	char* outstring;
	newfmt(&outstring, _JSON_FORMAT_TODAY,DataStorage::giveTodayDate().c_str(), temp, press, rain, forecast.c_str());
	//std::cout << "Statistics stored in JSON format" << '\n';
	_statJSON.assign(outstring, strlen(outstring));
	free(outstring); 
}

//For archive data
void WeatherData::makeJSONArchive(std::string date,float temp, float press, float rain) {
	char* outstring;
	newfmt(&outstring, _JSON_FORMAT_ARCHIVE, date.c_str(), temp, press, rain);
	//std::cout << "Statistics stored in JSON format" << '\n';
	_statJSON.assign(outstring, strlen(outstring));
	free(outstring);
}

std::string WeatherData::returnJSON() {
	return _statJSON;
}

float WeatherData::getAvg(std::vector<float> vector) {
	float res = 0;
	for (float val : vector) {
		res += val;
	}
	res /= vector.size();
	return res;
}

std::string WeatherData::simpleWeatherForecast(std::vector<float> pressures) {
	float slope;
	std::string res;
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
			res = "The weather is stable";
		}
		else {
			if (slope < 0) {
				res = "Weather won't be clement";
			}
			else {
				res = "Weather will turn clear and sunny";
			}
		}
	}
	else {
		res = "More data is needed";
	}
	return res;
}

void WeatherData::ShowStats() {
	//Get the statistics:
	std::string stats = returnStats();
	//Display statistics
	//std::cout << stats << '\n';
}

std::string WeatherData::returnStats() {
	//Calculate the statistics:
	float meanPress = getAvg(_arrPress);
	float meanTemp = getAvg(_arrTemp);
	float meanRain = getAvg(_arrRain);
	std::string forecast = simpleWeatherForecast(_arrPress);
	std::ostringstream result;
	result << "The average barometric pressure today is:" << meanPress << '\n';
	result << "The average temperature today is:" << meanTemp << '\n';
	result << "The average rain rate today is:" << meanRain << '\n';
	result <<  forecast << '\n';
	//Combine into a JSON string
	makeJSON(meanTemp, meanPress, meanRain, forecast);
	return result.str();
}


int WeatherData::extractData() {
	std::ifstream fp("downloaded.html");
	std::string strToFind = "<H1><center> Tableau d'observations pour <br>";
	std::string str;
	while (std::getline(fp, str)) {
		if (str.find(strToFind) != std::string::npos) {
			//std::cout << "Table with sensor measurements is found!" << '\n';
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
				//std::cerr << "Cannot convert to float" << std::endl;
				//std::cerr << "An exception occurred. Exception Nr. " << e << '\n';
			}
		}
		else {
			//std::cerr << "No match was found for temperature regex" << std::endl;
			//fprintf(stderr, "No match was found for temperature regex \n");
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
				//std::cerr << "Cannot convert to number" << std::endl;
				//std::cerr << "An exception occurred. Exception Nr. " << e << '\n';
			}
		}
		else {
			//std::cerr << "No match was found for barometric pressure regex" << std::endl;
			//fprintf(stderr, "No match was found for barometric pressure regex \n");
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
				//std::cerr << "Cannot convert to float" << std::endl;
				//std::cerr << "An exception occurred. Exception Nr. " << e << '\n';
			}
		}
		else {
			//std::cerr << "No match was found for rain regex" << std::endl;
			//fprintf(stderr, "No match was found for rain regex \n");
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
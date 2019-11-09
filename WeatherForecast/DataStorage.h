#pragma once
//https://cppcodetips.wordpress.com/tag/including-sqlite-with-visual-studio/
//#include <sqlite3.h> 
//https://stackoverflow.com/questions/50395965/how-do-i-connect-to-an-sqlite-database-in-c-using-visual-studio-2017-on-window
#include "sqlite3.h""
// => had to turn off pre compiled headers: https://stackoverflow.com/questions/20113280/fatal-error-c1010-stdafx-h-in-visual-studio-how-can-this-be-corrected
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm> 
#ifdef _WIN32
#include <io.h> 
#define access    _access_s
#else
#include <unistd.h>
#endif
#include <ctime>
#include <iomanip>

#pragma comment (lib, "sqlite3")
class DataStorage {
public:	
	DataStorage();
	int storeToDB(std::vector<float> temperatures, std::vector<float> pressions, std::vector<float> rainrates);
	int CreateTable();
	int AddEntry(std::string timestamp, std::string temp, std::string press, std::string rain);
	int AddEntry(std::string timestamp, float temp, float press, float rain);
	float getAvgTemperature(std::string date);
	float getAvgRainRate(std::string date);
	float getAvgPressure(std::string date);
	float getAvgTemperatureToday();
	float getAvgRainRateToday();
	float getAvgPressureToday();
	float getAvg(std::string type, std::string date);
	void showStatistics();
	int SelectThisYear();

private:
	const char* _db_name;
	int callback(void *NotUsed, int argc, char **argv, char **azColName);
	int getTable();

};

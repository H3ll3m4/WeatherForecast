#pragma once

#include <string>
#include <iterator> 
#include <map> 
#include <iostream>
#include <vector>
#include <iterator>
#include "WeatherData.h"
#include "DataStorage.h";


const std::string ENV[24] = {
	"COMSPEC", "DOCUMENT_ROOT", "GATEWAY_INTERFACE",
	"HTTP_ACCEPT", "HTTP_ACCEPT_ENCODING",
	"HTTP_ACCEPT_LANGUAGE", "HTTP_CONNECTION",
	"HTTP_HOST", "HTTP_USER_AGENT", "PATH",
	"QUERY_STRING", "REMOTE_ADDR", "REMOTE_PORT",
	"REQUEST_METHOD", "REQUEST_URI", "SCRIPT_FILENAME",
	"SCRIPT_NAME", "SERVER_ADDR", "SERVER_ADMIN",
	"SERVER_NAME","SERVER_PORT","SERVER_PROTOCOL",
	"SERVER_SIGNATURE","SERVER_SOFTWARE" };


class SimpleCGI {
public:
	std::map<std::string, std::string> _qm;
	SimpleCGI();
	void displayHTML(std::string data);
	std::vector<std::string> split(const std::string& s, char delimiter);
	void executeQuery();
	bool checkCGI();

private:
	std::string getQuery();
	void cgiQueryParser(std::string queryString);
	//const std::string ENV[24];
};
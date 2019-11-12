#include "SimpleCGI.h"

SimpleCGI::SimpleCGI()
{
	cgiQueryParser(getQuery());
}

void SimpleCGI::executeQuery() {
	std::string date = _qm["date"];
	WeatherData wd(date);
	if (checkCGI())
		displayHTML(wd.returnStats());
	else {
		std::cout << "Result formatted in HTML:" << '\n';
		displayHTML(wd.returnStats());
		std::cout << "Result formatted in JSON:" << '\n';
		std::cout << wd.returnJSON() << '\n';
	}
}

std::vector<std::string> SimpleCGI::split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void SimpleCGI::cgiQueryParser(std::string queryString) {
	using namespace std;	
	vector<string> queries = split(queryString, '&');
	for (string q : queries) {
		vector<string> qv = split(q,'=');
		if (qv.size() >= 2) {
			string key = qv[0];
			string val = qv[1];
			if (!key.empty()) {
				_qm[key] = !val.empty() ? val : "";
			}
		}
	}
	//fprintf(stdout, "Query Map is: \n");
	//for (auto it = _qm.cbegin(); it != _qm.cend(); ++it)
	//{
	//	std::cout << it->first << " " << it->second << "\n";
	//}
}

bool SimpleCGI::checkCGI() {
	char * res = getenv(ENV[10].c_str());
	if (res == NULL) return false;
	else return true;
}

//Query is the xxxx in url such as /cgi-bin/WeatherForecast.cgi?date=XXXX-XX-XX&whatever...
std::string SimpleCGI::getQuery() {
	try {
		if (checkCGI()) {
			std::string query(getenv(ENV[10].c_str()));
			//std::cout << "Query from CGI script is: " << query << '\n';
			//test if query is empty
			if (!query.empty()) {
				return query; 
			}
			else {
				//std::cerr << "CGI variables exists but query string is empty " << '\n';
			}
		}
		else {
			//std::cout << "No return from the CGI environment variables" << '\n';
		}
	}
	catch (int e) {
		//std::cerr << "Cannot get the query from CGI so default query will be used" << std::endl;
		//std::cerr << "An exception occurred. Exception Nr. " << e << '\n';
	}
	std::string query = "date=" + DataStorage::giveTodayDate();
	//std::cout << "Query is: " << query << '\n';
	return query;
}

void SimpleCGI::displayHTML(std::string data) {
	using namespace std;
	cout << "Content-type:text/html\r\n\r\n";
	cout << "<html>\n";
	cout << "<head>\n";
	cout << "<title>Statistics Weather in Sain Bel</title>\n";
	cout << "</head>\n";
	cout << "<body>\n";
	cout << "<h2>" << data << "</h2>\n";
	cout << "</body>\n";
	cout << "</html>\n";
}

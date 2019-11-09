#include "DataStorage.h"

DataStorage::DataStorage() {
	_db_name = "weather.db";
}


bool FileExists(const std::string &Filename)
{
	return access(Filename.c_str(), 0) == 0;
}

std::string giveDate() {
	tm *tptr;
	std::time_t t = std::time(nullptr);
	tptr = std::gmtime(&t);
	//We want time like this: 2019-01-01 00:02:14
	std::ostringstream oss;
	oss << std::put_time(tptr, "%Y-%m-%d %H-%M-%S");
	auto datetime = oss.str();
	//std::cout << datetime << std::endl;
	std::string date = datetime.substr(0,10);
	//std::cout << "Date returned is:" << date << '\n';
	return date;
}

int DataStorage::getTable() {

	char* szError = 0;
	char** paszResults = 0;
	int nRows(0);
	int nCols(0);
	std::string sql = "SELECT * FROM WEATHER";
	int ret;
	sqlite3 *db;
	ret = sqlite3_open(_db_name, &db);

	if (ret) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(0);
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
	}
	ret = sqlite3_get_table(db, sql.c_str(), &paszResults, &nRows, &nCols, &szError);
	if (ret != SQLITE_OK) {
		std::cout << "Cannot get the table weather" << std::endl;
	}
	return ret;
}



int DataStorage::storeToDB(std::vector<float> temperatures, std::vector<float> pressions, std::vector<float> rainrates) {
	int error = 0;
	//Check if table exists
	if (!FileExists(_db_name)) {
		//Otherwise create the table
		error = DataStorage::CreateTable();
	}
	if (getTable() != SQLITE_OK) {
		error = DataStorage::CreateTable();
	}

	//Iteration over the vectors content to add them to DB
	//All vectors should be the same size but let's take the min
	int m = std::min(std::min(temperatures.size(), rainrates.size()), pressions.size());
	try {
		for (int i = 0; i < temperatures.size();i++) {
			std::ostringstream hour;
			hour  << std::setw(2) << std::setfill('0') << i;
			std::string timestamp = giveDate()+" "+hour.str()+"-00-00";
			AddEntry(timestamp, temperatures[i], pressions[i], rainrates[i] );
		}
	}
	catch (int e) {
		std::cout << "Cannot store to database" << std::endl;
		std::cout << "An exception occurred. Exception Nr. " << e << '\n';
	}


	return error;
}

int DataStorage::callback(void *NotUsed, int argc, char **argv, char **azColName) {
	int i;
	for (i = 0; i<argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int DataStorage::CreateTable() {
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	std::string sql;
	int error = 0;

	// Open database 
	rc = sqlite3_open(_db_name, &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(-1);
	}
	else {
		fprintf(stdout, "Opened database successfully\n");
	}

	// Create SQL statement 
	sql = "CREATE TABLE WEATHER("  \
		"TIMESTAMP           DATETIME    NOT NULL," \
		"TEMPERATURE           REAL    NOT NULL," \
		"PRESSURE            REAL     NOT NULL," \
		"RAIN         REAL );";

	//std::cout << "SQL query is:" << sql << '\n';
	// Execute SQL statement 
	rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		error = -1;
	}
	else {
		fprintf(stdout, "Table created successfully\n");
	}

	sqlite3_close(db);
	return error;
}


int DataStorage::AddEntry(std::string td, std::string temp, std::string press, std::string rain) {
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	std::string sql;
	int error = 0;

	// Open database 
	rc = sqlite3_open(_db_name, &db);

	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return(-1);
	}
	else {
		fprintf(stdout, "Opened database successfully\n");
	}

	// Create SQL statement
	sql = "INSERT INTO WEATHER (TIMESTAMP,TEMPERATURE,PRESSURE,RAIN) " \
		"VALUES ('"+ td+ "',"+ temp +  "," + press + "," + rain + " ); ";
	//std::cout << "SQL query is:" << sql << '\n';

	// Execute SQL statement 
	rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		fprintf(stdout, "Records created successfully\n");
	}
	sqlite3_close(db);
	return error;
}

int DataStorage::AddEntry(std::string timestamp, float temp, float press, float rain)
{
	return AddEntry(timestamp, std::to_string(temp),std::to_string(press),std::to_string(rain));
}

int DataStorage::SelectThisYear()
{
	sqlite3* pDb = NULL;
	sqlite3_stmt* query = NULL;
	int ret = 0;
	do // avoid nested if's
	{
		// initialize engine
		if (SQLITE_OK != (ret = sqlite3_initialize()))
		{
			printf("Failed to initialize library: %d\n", ret);
			break;
		}
		// open connection to a DB
		if (SQLITE_OK != (ret = sqlite3_open_v2(_db_name, &pDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)))
		{
			printf("Failed to open conn: %d\n", ret);
			break;
		}
		// prepare the statement
		//if (SQLITE_OK != (ret = sqlite3_prepare_v2(pDb, "SELECT * FROM WEATHER WHERE YEAR(TIMESTAMP) = YEAR(CURDATE())", -1, &query, NULL)))
		//if (SQLITE_OK != (ret = sqlite3_prepare_v2(pDb, "SELECT * FROM WEATHER WHERE YEAR(TIMESTAMP) = YEAR(GETDATE())", -1, &query, NULL)))
		if (SQLITE_OK != (ret = sqlite3_prepare_v2(pDb, "SELECT * FROM WEATHER WHERE YEAR(TIMESTAMP) = YEAR(DATETIME('now'))", -1, &query, NULL)))
		{
			printf("Failed to prepare insert: %d, %s\n", ret, sqlite3_errmsg(pDb));
			break;
		}
		// step to 1st row of data
		if (SQLITE_ROW != (ret = sqlite3_step(query))) 
		{
			printf("Failed to step: %d, %s\n", ret, sqlite3_errmsg(pDb));
			break;
		}
		// ... and print the value of column 0 (expect 2012 here)
		printf("Value from sqlite: %s", sqlite3_column_text(query, 0));

	} while (false);
	// cleanup
	if (NULL != query) sqlite3_finalize(query);
	if (NULL != pDb) sqlite3_close(pDb);
	sqlite3_shutdown();
	return ret;
}

float DataStorage::getAvg(std::string type, std::string date)
{
	using namespace std;
	string date_like = date + "%";
	//string sql = "SELECT AVG("+type+") FROM WEATHER WHERE TIMESTAMP LIKE " + date;
	string sql = "SELECT AVG(" + type + ") FROM WEATHER";
	//string sql = "SELECT AVG(" + type + ") FROM WEATHER WHERE TIMESTAMP LIKE " + date_like;
	//string sql = "SELECT AVG("+type+") FROM WEATHER WHERE TRUNK(TIMESTAMP) = TO_DATE('"+date+"','YYYY-MM-DD')";
	cout << "SQL query is:" << sql << '\n';
	sqlite3* pDb = NULL;
	sqlite3_stmt* query = NULL;
	float ret = 0;
	do // avoid nested if's
	{
		// initialize engine
		if (SQLITE_OK != (ret = sqlite3_initialize()))
		{
			printf("Failed to initialize library: %d\n", ret);
			break;
		}
		// open connection to a DB
		if (SQLITE_OK != (ret = sqlite3_open_v2(_db_name, &pDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL)))
		{
			printf("Failed to open conn: %d\n", ret);
			break;
		}
		// prepare the statement
		if (SQLITE_OK != (ret = sqlite3_prepare_v2(pDb, sql.c_str(), -1, &query, NULL)))
		{
			printf("Failed to prepare insert: %d, %s\n", ret, sqlite3_errmsg(pDb));
			break;
		}
		// step to 1st row of data
		if (SQLITE_ROW != (ret = sqlite3_step(query))) // see documentation, this can return more values as success
		{
			printf("Failed to step: %d, %s\n", ret, sqlite3_errmsg(pDb));
			break;
		}
		// ... and print the value of column 0 (expect 2012 here)
		printf("Value from sqlite: %s", sqlite3_column_text(query, 0));
	} while (false);
	const unsigned char * res = sqlite3_column_text(query, 0);
	//string retn(res);
	//string retn(reinterpret_cast<char*>(res));
	string retn((char*)res);
	//string retn;
	//retn = string(res);
	ret = stof(retn);
	// cleanup
	if (NULL != query) sqlite3_finalize(query);
	if (NULL != pDb) sqlite3_close(pDb);

	return ret;
}

float DataStorage::getAvgTemperature(std::string date) {
	return getAvg("TEMPERATURE", date);
}

float DataStorage::getAvgRainRate(std::string date) {
	return getAvg("RAIN", date);
}

float DataStorage::getAvgPressure(std::string date) {
	return getAvg("PRESSURE", date);
}

float DataStorage::getAvgTemperatureToday()
{
	return getAvgTemperature(giveDate());
}

float DataStorage::getAvgRainRateToday()
{
	return getAvgRainRate(giveDate());
}

float DataStorage::getAvgPressureToday()
{
	return getAvgPressure(giveDate());
}

void DataStorage::showStatistics() {
	std::cout << "The average temperature today is:" << getAvgTemperatureToday() << '\n';
	std::cout << "The average barometric pressure today is:" << getAvgPressureToday() << '\n';
	std::cout << "The average rain rate today is:" << getAvgRainRateToday() << '\n\n';
}

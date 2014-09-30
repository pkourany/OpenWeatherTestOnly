/**
 * This is a library for the openweathermap.org api, made for spark core (http://spark.io).
 *
 * It uses the API described at http://openweathermap.org/API
 */

#ifndef _SPARKWEATHER
#define _SPARKWEATHER

#include "application.h"
#include "HttpClient.h"
#include "JsonParser.h"


typedef struct forecast_weather_response_t {
	float temp_high;
	float temp_low;
	String descr;
	int conditionCode; // see http://openweathermap.org/wiki/API/Weather_Condition_Codes
	bool isSuccess;
	// defaults:
	forecast_weather_response_t(): temp_high(255), temp_low(255), conditionCode(-1), isSuccess(false) {};
} forecast_weather_response_t;

typedef struct current_weather_response_t {
	float temp_now;
	float humi_now;
	float press_now;
	String descr;
	int conditionCode; // see http://openweathermap.org/wiki/API/Weather_Condition_Codes
	bool isSuccess;
	// defaults:
	current_weather_response_t(): temp_now(255), humi_now(255), press_now(0), conditionCode(-1), isSuccess(false) {};
} current_weather_response_t;

class Weather {
public:
	Weather(String location, HttpClient* client, String apiKey);
	forecast_weather_response_t updateF();
	current_weather_response_t updateC();
	void setCelsius();
	void setFahrenheit();

	// for cache:
	forecast_weather_response_t cachedUpdateF();
	current_weather_response_t cachedUpdateC();

private:
	JsonParser<200> parser; // occupies 70 * 4 bytes

	http_request_t request;
	String location;
	String apiKey;
	String unitsForTemperature;
	HttpClient* client;
//	forecast_weather_response_t responseF;
//	current_weather_response_t responseC;
	forecast_weather_response_t parseF(String json);
	current_weather_response_t parseC(String json);
	String readData(String s, String needle, String endMark);
	int readIntField(const char* fieldname, String jsonData);
	String readTextField(const char* fieldname, String jsonData);

	long parseInt(String s);

	// cache:
	unsigned long weather_sync_interval;
	unsigned long lastsyncF, lastsyncC;
	forecast_weather_response_t lastReponseF;
	current_weather_response_t lastReponseC;
	
};

#endif

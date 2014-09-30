#include "openweathermap.h"

//extern int freeMemory();
//extern int freew, freef;
//extern String msg;

/** Show temp in degree celsius */
void Weather::setCelsius() {
	this->unitsForTemperature = "metric";
}
void Weather::setFahrenheit() {
	this->unitsForTemperature = "imperial";
}

Weather::Weather(String location, HttpClient* client, String apiKey) {
	this->location = location;
	this->client = client;
	this->apiKey = apiKey;

	// default:
	setCelsius();

	// init cache
	this->lastsyncF = 0;
	this->lastsyncC = 0;
	this->weather_sync_interval = 1000 * 3600 * 2; // milliseconds

}

forecast_weather_response_t Weather::updateF() {
	//Serial.print("loading forecast weather with url: ");
	//Serial.println(request.path);
//	extern uint16_t countf;
	
	request.hostname = "api.openweathermap.org";
	request.port = 80;
	request.path = "/data/2.5/forecast/daily?q=" //
	+ location // e.g. "Berlin,de"
			+ "&units=" + unitsForTemperature // metric or imperial
			+ "&cnt=1" // number of days
			+ "&mode=json" // xml or json
			+ "&APPID=" + apiKey; // see http://openweathermap.org/appid
	request.body = "";
	
	http_response_t responseF;
	this->client->get(request, responseF);
	
	if (responseF.status == 200) {
		return parseF(responseF.body);
	} else {
//		countf++;
//		msg = "Forecast response error";
		//Serial.print("forecast request failed ");
		forecast_weather_response_t responseFailed;
		return responseFailed;
	}
}

current_weather_response_t Weather::updateC() {
	//Serial.print("loading current weather with url: ");
	//Serial.println(request.path);
//	extern uint16_t countc;
	
	request.hostname = "api.openweathermap.org";
	request.port = 80;
	request.path = "/data/2.5/weather?q=" //
	+ location // e.g. "Berlin,de"
			+ "&units=" + unitsForTemperature // metric or imperial
			+ "&mode=json" // xml or json
			+ "&APPID=" + apiKey; // see http://openweathermap.org/appid
	request.body = "";

	http_response_t responseC;
	this->client->get(request, responseC);

	if (responseC.status == 200) {
		return parseC(responseC.body);
	} else {
//		countc++;
//		msg = "Weather response error";
		//Serial.print("weather request failed ");
		current_weather_response_t responseFailed;
		return responseFailed;
	}
}


/**
 *
 */
forecast_weather_response_t Weather::parseF(String data) {
	/*
	 * example:
	 * http://api.openweathermap.org/data/2.5/forecast/daily?q=London,uk&units=metric&cnt=1
	 *
	 * {"cod":"200","message":0.3348,
	 * "city":{"id":2643743,"name":"London",
	 * "coord":{"lon":-0.12574,"lat":51.50853},
	 * "country":"GB","population":0,"sys":{"population":0}},"cnt":1,
	 * "list":[{"dt":1396267200,
	 * "temp":{"day":14.69,"min":8.6,"max":14.69,"night":8.6,"eve":14.69,"morn":14.69},
	 * "pressure":1019.37,"humidity":66,
	 * "weather":[{"id":803,"main":"Clouds","description":"broken clouds","icon":"04d"}],
	 * "speed":3.7,"deg":162,"clouds":64}]}
	 */

	forecast_weather_response_t response;
	unsigned char buffer[600];
	data.getBytes(buffer, sizeof(buffer), 0);
	JsonHashTable root = parser.parseHashTable((char*) buffer);
	if (!root.success()) {
		//Serial.println("Parsing fail: could be an invalid JSON, or too many tokens");
//		Serial1.print("TT");
//		Serial1.print((char *) buffer);
		return response;
	}

	JsonArray daysList = root.getArray("list");
	JsonHashTable today = daysList.getHashTable(0);
	JsonHashTable temp = today.getHashTable("temp");
	response.temp_low = temp.getDouble("min");
	response.temp_high = temp.getDouble("max");

	JsonHashTable weather = today.getArray("weather").getHashTable(0);
	response.descr = weather.getString("description");
	response.conditionCode = weather.getLong("id");

	// a value over 250 degree/fahrenheit makes no sense
	response.isSuccess = true;
	return response;
}

/**
 *
 */
current_weather_response_t Weather::parseC(String data) {
	/*
	 * example:
	 * http://api.openweathermap.org/data/2.5/forecast/daily?q=London,uk&units=metric&cnt=1
	 *
	 * {"coord":{"lon":-75.69,"lat":45.42},
	 * "sys":{"message":0.0351,"country":"Canada","sunrise":1396866626,"sunset":1396913939},
	 * "weather":[{"id":802,"main":"Clouds","description":"scattered clouds","icon":"03d"}],
	 * "base":"cmc stations",
	 * "main":{"temp":4.72,"humidity":50,"pressure":1019.773,"temp_min":2,"temp_max":7.3},
	 * "wind":{"speed":2,"gust":3.1,"deg":73},
	 * "clouds":{"all":48},
	 * "dt":1396881928,
	 * "id":6094817,
	 * "name":"Ottawa",
	 * "cod":200}
	 */

	current_weather_response_t response;
	unsigned char buffer[600];
	data.getBytes(buffer, sizeof(buffer), 0);
	JsonHashTable root = parser.parseHashTable((char*) buffer);
	if (!root.success()) {
		//Serial.println("Parsing fail: could be an invalid JSON, or too many tokens");
//		Serial1.print("TT");
//		Serial1.print((char *) buffer);
		return response;
	}

	JsonHashTable weather = root.getArray("weather").getHashTable(0);
	response.descr = weather.getString("description");
	response.conditionCode = weather.getLong("id");

	JsonHashTable nowlist = root.getHashTable("main");
	response.temp_now = nowlist.getDouble("temp");
	response.humi_now = nowlist.getDouble("humidity");
	response.press_now = nowlist.getDouble("pressure");

	// a value over 250 degree/fahrenheit makes no sense
	response.isSuccess = true;
	return response;
}


/**
 * Reads from the cache if there is a fresh and valid response.
 */
forecast_weather_response_t Weather::cachedUpdateF() {
	if (lastsyncF == 0 || (lastsyncF + weather_sync_interval) < millis()) {
		lastReponseF = this->updateF();
		if (lastReponseF.isSuccess) {
			lastsyncF = millis();
		}
	} else {
		//Serial.println("using cached weather");
	}
	return lastReponseF;
}

/**
 * Reads from the cache if there is a fresh and valid response.
 */
current_weather_response_t Weather::cachedUpdateC() {
	if (lastsyncC == 0 || (lastsyncC + weather_sync_interval) < millis()) {
		lastReponseC = this->updateC();
		if (lastReponseC.isSuccess) {
			lastsyncC = millis();
		}
	} else {
		//Serial.println("using cached weather");
	}
	return lastReponseC;
}

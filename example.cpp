#include "application.h"
//#include "spark_disable_cloud.h"

#include "openweathermap.h"
#include "HttpClient.h"


#define _Digole_Serial_UART_
#include "digoleGeo.h"
DigoleSerialDisp digole(&Serial1, 115200);

String msg = "";
String httpmsg = "";

//unsigned int nextTime = 0;    // next time to contact the server
//uint16_t countf, countc;
//unsigned long timer, timerc, timerf;
//int freew, freef;

Weather* weather;
HttpClient* httpClient;

//unsigned int __heap_start;
//void *__brkval;

/*
* The free list structure as maintained by the 
* avr-libc memory allocation routines.
*
struct __freelist {
	size_t sz;
	struct __freelist *nx;
};

// The head of the free list structure
struct __freelist *__flp;

int freeMemory();
// Calculates the size of the free list

int freeListSize() {
	struct __freelist* current;
	int total = 0;

	for (current = __flp; current; current = current->nx) {
		total += 2; // Add two bytes for the memory block's header
		total += (int) current->sz;
	}

	return total;
}

int freeMemory() {
	int free_memory;

	if ((int)__brkval == 0) {
		free_memory = ((int)&free_memory) - ((int)&__heap_start);
	} else {
		free_memory = ((int)&free_memory) - ((int)__brkval);
		free_memory += freeListSize();
	}
	return free_memory;
}
*/


void setup() {
	//Serial.begin(9600);
	
	digole.begin();					//Init digole, clear display and set to font #10
	//digole.backLightOff();
	digole.setContrast(40);
	
	//digole.clearScreen();
	digole.setFont(6);
	//Time.zone(-4);

	httpClient = new HttpClient();
	weather = new Weather("Ottawa,ON", httpClient,
	"62810bbd5419f1ff67f8f61588833bf6");
	weather->setCelsius();
	
//	countf = countc = 0;
//	timer = millis();
}

void loop() {
//	if (millis() >= nextTime) {
		
		delay(5000);
		forecast_weather_response_t forecast = weather->updateF(); //weather->cachedUpdateF();
/*		if (!forecast.isSuccess) {
			digole.clearScreen();
			digole.setFont(6);
			digole.println(httpmsg);
			delay(10000);
		} */
		delay(5000);
		current_weather_response_t current = weather->updateC(); //weather->cachedUpdateC();
/*		if (!current.isSuccess) {
			digole.clearScreen();
			digole.setFont(6);
			digole.println(httpmsg);
			delay(10000);
		} */

		if (forecast.isSuccess && current.isSuccess) {
	
			digole.clearScreen();
			digole.setFont(10);
			digole.print("OTTAWA NOW: ");
			digole.print(current.temp_now,1);
			digole.print("C");
			digole.println();
			digole.setFont(6);
			digole.print("Humidity: ");
			digole.print(current.humi_now,0);
			digole.print("% Pressure: ");
			digole.println(current.press_now,0);
			digole.print("-> ");
			digole.println(current.descr);
			digole.println();
			digole.setFont(10);
			digole.println("FORECAST");
			digole.setFont(6);
			digole.print("Low: ");
			digole.print(forecast.temp_low,1);
			digole.print("C  High: ");
			digole.print(forecast.temp_high,1);
			digole.println("C");
			digole.print("-> ");
			digole.println(forecast.descr);
//			digole.println(); 
//			digole.println(Time.timeStr(resp.dt));
/*			digole.print(millis() - timer - 2000UL);
			digole.print(" / ");
			digole.print(timerf);
			digole.print(" / ");
			digole.print(countf);
			digole.print(" / ");
			digole.print(timerc);
			digole.print(" / ");
			digole.print(countc);
*/
//			digole.print(httpmsg);
//			digole.print(msg);
			delay(1000);
//			timer=millis();

			Spark.sleep(SLEEP_MODE_DEEP, 60*30);
		}
		// check again in 5 seconds:

//		nextTime = millis() + 10000;

//	}
}

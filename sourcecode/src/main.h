#include <Arduino.h>
#include <Wire.h>

// Connect to Wi-Fi
/*
	create a file named secrets.h in the folder src.
	it should contain your wifi network (ssid) and your wifi password:
	const char *ssid = "";
	const char *pass = "";
*/

//Prototypes:
//Connect to WiFi Network described in secrets.h
void connect_to_wifi();
//Print the connection details on Serial Monitor
void print_connection();
//Update the RealtimeClock with NTP Server
//Gets epochTime from Timeserver and stores the value in the RTC
void RTC_Update();
//Print formatted Timestring on Serial Monitor
void printDateTime(time_t t, const char *tz);
//
void WS2812B_Write_Time();

void WS2812B_Write_Number(uint16_t startIndex, uint8_t number);

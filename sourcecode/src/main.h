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
void connect_to_wifi();
void print_connection();
void RTC_Update();
bool RTC_Valid();
void printDateTime();

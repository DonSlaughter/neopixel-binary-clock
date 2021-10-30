#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <RtcDS3231.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <FastLED.h>
#include "secrets.h"
#include "main.h"


// Define NTP Client to get time
WiFiUDP ntpUDP;
//NTPClient updates normall every minute
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

//Changing Timezone and using Daylightsaving
TimeChangeRule mySTD = {"CET", Last, Sun, Oct, 3, 60};
TimeChangeRule myDST = {"CEST", Last, Sun, Mar, 2, 120};
Timezone myTZ(myDST, mySTD);
TimeChangeRule *tcr;

RtcDS3231<TwoWire> Rtc(Wire);
//SCL pin D1
//SDA pin D2

//time_t sctructs to store timevalues
time_t utc;
time_t local;


void setup() {
	Serial.begin(115200);
	delay(250);
	connect_to_wifi();
	print_connection();
	timeClient.begin();
	delay(250);
	Rtc.Begin();
	RTC_Update();
	delay(1000);
}

void loop(){
	utc =  Rtc.GetDateTime();
	local = myTZ.toLocal(utc, &tcr);
	printDateTime(local, tcr -> abbrev);
}

void connect_to_wifi(){
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, pass);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

}

void print_connection(){
	Serial.println();
	Serial.print("Connected to: ");
	Serial.println(ssid);
	Serial.print("Local IP-Adress: ");
	Serial.println(WiFi.localIP());
}

void RTC_Update(){
	timeClient.update();
	unsigned long epochTime = timeClient.getEpochTime();
	Rtc.SetDateTime(epochTime);
}

void printDateTime(time_t t, const char *tz){
	char buf[32];
	char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
	strcpy(m, monthShortStr(month(t)));
	sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
		hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
	Serial.println(buf);
}

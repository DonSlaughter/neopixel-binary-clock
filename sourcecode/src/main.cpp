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

#define LED_PIN D5
#define NUM_LEDS 63
#define COLOR_ORDER GBR

CRGB leds[NUM_LEDS];

void setup()
{
	Serial.begin(115200);
	delay(10);
	FastLED.addLeds<WS2812B, LED_PIN>(leds, NUM_LEDS);
	connect_to_wifi();
	print_connection();
	timeClient.begin();
	delay(25);
	Rtc.Begin();
	RTC_Update();
	delay(10);
}

void loop()
{
	utc =  Rtc.GetDateTime();
	local = myTZ.toLocal(utc, &tcr);
	printDateTime(local, tcr -> abbrev);
	WS2812B_Write_Time();
}

void connect_to_wifi()
{
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, pass);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

}

void print_connection()
{
	Serial.println();
	Serial.print("Connected to: ");
	Serial.println(ssid);
	Serial.print("Local IP-Adress: ");
	Serial.println(WiFi.localIP());
}

void RTC_Update()
{
	timeClient.update();
	unsigned long epochTime = timeClient.getEpochTime();
	Rtc.SetDateTime(epochTime);
}

void printDateTime(time_t t, const char *tz)
{
	char buf[32];
	char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
	strcpy(m, monthShortStr(month(t)));
	sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
		hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
	Serial.println(buf);
}

void WS2812B_Write_Time()
{
	WS2812B_Write_Number(0, second(local));
	WS2812B_Write_Number(8, minute(local));
	WS2812B_Write_Number(16, hour(local));
	WS2812B_Write_Number(24, day(local));
	WS2812B_Write_Number(32, month(local));
	FastLED.show();

}

void WS2812B_Write_Number(uint16_t startIndex, uint8_t number)
{
	for (uint16_t i = 0; i !=8; i++) {
		uint8_t mask = 1 << i;
		uint16_t pixelID = startIndex +i;
		if ((number & mask) != 0) {
			leds[pixelID] = CRGB::Red;
		}
		else {
			leds[pixelID] = CRGB::Black;
		}
	}
}

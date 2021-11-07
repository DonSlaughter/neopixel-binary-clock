#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <RtcDS3231.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
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
time_t old_local;

//#define LED_PIN D5
#define NUM_LEDS 26
#define COLOR_ORDER BRG
const int LED_PIN = D5;
const uint8_t ColorCount = 5;

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(NUM_LEDS, LED_PIN);
HslColor Black(0.0, 0.0, 0.0);
HslColor Colors[ColorCount];
const float distance = 1.0 / (float)ColorCount;
const float step = 0.0001;

unsigned long ntp_start_time;
//Wait 10 seconds for NTP-Server
unsigned long ntp_wait_time = 10000;

void setup()
{
	Serial.begin(115200);
	delay(10);
	connect_to_wifi();
	print_connection();
	timeClient.begin();
	delay(25);
	Rtc.Begin();
	ntp_start_time = millis();
	//RTC_Update();
	utc = Rtc.GetDateTime();
	old_local = myTZ.toLocal(utc, &tcr);
	delay(10);
	for (uint16_t i = 0; i != ColorCount; ++i) {
		Colors[i] = HslColor(i * distance, 1.0, 0.03);
	}
	strip.Begin();
	strip.Show();
}

void loop()
{
	utc =  Rtc.GetDateTime();
	local = myTZ.toLocal(utc, &tcr);
	if (second(local) != second(old_local)){
		printDateTime(local, tcr -> abbrev);
		WS2812B_Write_Time();
		old_local = myTZ.toLocal(utc, &tcr);
		for (int i = 0; i != ColorCount; ++i) {
			if (Colors[i].H < 1 - step)
				Colors[i].H += step;
			else
				Colors[i].H = 0;
		}
	}
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
	Serial.println("Time Set");
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
	WS2812B_Write_Number(0, 6, second(local), Colors[0]);
	WS2812B_Write_Number(6, 12, minute(local), Colors[2]);
	WS2812B_Write_Number(12, 17, hour(local), Colors[4]);
	WS2812B_Write_Number(17, 22, day(local), Colors[1]);
	WS2812B_Write_Number(22, 25, month(local), Colors[3]);
	strip.Show();
}

void WS2812B_Write_Number(uint16_t startIndex, uint16_t endIndex, uint8_t number, HslColor color)
{
	for (uint16_t i = 0; i !=endIndex; i++) {
		uint8_t mask = 1 << i;
		uint16_t pixelID = startIndex +i;
		if ((number & mask) != 0) {
			strip.SetPixelColor(pixelID, color);
			//leds[pixelID] = CHSV(color,255,255);
		}
		else {
			strip.SetPixelColor(pixelID, Black);
			//leds[pixelID] = CHSV(0,0,0);
		}
	}
}

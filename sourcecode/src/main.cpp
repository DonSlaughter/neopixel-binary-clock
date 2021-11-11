#include <Arduino.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <TimeLib.h>
#include <Timezone.h>
//#define FASTLED_ALLOW_INTERRUPTS 0
//#define INTERRUPT_THRESHOLD 1
//#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>
#include "secrets.h"
#include "main.h"
#define WITH_WLAN 0
#ifdef WITH_WLAN
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#endif


// Define NTP Client to get time
//WiFiUDP ntpUDP;
//NTPClient updates normall every minute
//NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

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

#define LED_PIN D5
#define NUM_LEDS 26
#define COLOR_ORDER BRG

int a;
CRGB leds[NUM_LEDS];
int b;

void setup()
{
	Serial.begin(115200);
	//while (!Serial){;}
	FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	//connect_to_wifi();
	//print_connection();
	//timeClient.begin();
	delay(25);
	Rtc.Begin();
	//RTC_Update();
	utc = Rtc.GetDateTime();
	old_local = myTZ.toLocal(utc, &tcr);
	//old_local = time(NULL);
	delay(10);
}

void loop()
{
	utc =  Rtc.GetDateTime();
	local = myTZ.toLocal(utc, &tcr);
	if (second(local) != second(old_local)){
		printDateTime(local, tcr -> abbrev);
		WS2812B_Write_Time();
		old_local = myTZ.toLocal(utc, &tcr);
		delay(10);
	}
}

#if WITH_WLAN
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
	timeClient.end();
}
#endif

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
	WS2812B_Write_Number(0, 5, second(local), 120);
	WS2812B_Write_Number(6, 11, minute(local), 0);
	WS2812B_Write_Number(12, 16, hour(local), 90);
	WS2812B_Write_Number(17, 21, day(local), 247);
	WS2812B_Write_Number(22, 25, month(local), 190);
	FastLED.show();
}

void WS2812B_Write_Number(uint16_t startIndex, uint16_t endIndex, uint8_t number, uint8_t color)
{
	uint16_t mask = 1;
	for (uint16_t i = startIndex ; i <= endIndex ; i++) {
		if ((number & mask) != 0) {
			leds[i] = CHSV(color,255,20);
		}
		else {
			leds[i] = CHSV(0,0,0);
		}
		mask = mask << 1;
	}
	//FastLED.show();
}

void test_one_led(uint8_t pixelID)
{
	leds[pixelID] = CRGB::Red;
	delay(1000);
	FastLED.show();
	leds[pixelID] = CRGB::Black;
	delay(1000);
	FastLED.show();
}

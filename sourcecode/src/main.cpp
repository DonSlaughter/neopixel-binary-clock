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
//Turning WiFi off or on. It seems there are som interrupt Bugs concerning the
//WiFi. If Wifi is turned off the Clock will work as expected. If it is turned
//on the first LED will flicker.
#define WITH_WLAN 0
#if WITH_WLAN
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Define NTP Client to get time
WiFiUDP ntpUDP;
//NTPClient updates normall every minute
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");
#endif

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
time_t night_time;

//LED Parameter and definitions
#define LED_PIN D5
#define NUM_LEDS 26
#define COLOR_ORDER BRG

const uint8_t saturation = 255;
uint8_t value = 20;
//Night Mode parameters:
uint8_t high_brightness = 100;
uint8_t dimmed_brightness = 20;
uint8_t start_hour = 20;
uint8_t stop_hour = 7;

CHSV Colors[5];
CHSV Black(0,0,0);

CRGB leds[NUM_LEDS];

bool night_mode = true;


void setup()
{
	Serial.begin(115200);
	delay(250);
	FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
	//test_all_leds();
	Colors[0] = CHSV(0, saturation, value);
	Colors[1] = CHSV(51, saturation, value);
	Colors[2] = CHSV(102, saturation, value);
	Colors[3] = CHSV(153, saturation, value);
	Colors[4] = CHSV(204, saturation, value);

	Rtc.Begin();
#if WITH_WLAN
	connect_to_wifi();
	print_connection();
	timeClient.begin();
	RTC_Update();
#endif
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
		night_mode_on();
		WS2812B_Write_Time();
		old_local = myTZ.toLocal(utc, &tcr);
		change_colors();
		//delay(10);
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

void change_colors()
{
	uint8_t gHueDelta = 1;
	for (uint8_t i = 0; i < 5; i++) {
		Colors[i].hue += gHueDelta;
		Colors[i].val = value;
	}
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
	WS2812B_Write_Number(0, 5, second(local), Colors[0]);
	WS2812B_Write_Number(6, 11, minute(local), Colors[2]);
	WS2812B_Write_Number(12, 16, hour(local), Colors[4]);
	WS2812B_Write_Number(17, 21, day(local), Colors[1]);
	WS2812B_Write_Number(22, 25, month(local), Colors[3]);
	FastLED.show();
}

void WS2812B_Write_Number(uint8_t startIndex, uint8_t endIndex, uint8_t number, CHSV color)
{
	uint8_t mask = 1;
	for (uint8_t i = startIndex ; i <= endIndex ; i++) {
		if ((number & mask) != 0) {
			leds[i] = color;
		}
		else {
			leds[i] = Black;
		}
		mask = mask << 1;
	}
}

void night_mode_on()
{
	if (night_mode == true) {
		if ((hour(local) >= start_hour) || ((hour(local) >= 0) && (hour(local) <= stop_hour))) {
			value = dimmed_brightness;
		}
		else {
			value = high_brightness;
		}
	}
}

//Debug Functions

void test_all_leds()
{
	char buf[20];
	Serial.print("Starting Test");
	for (uint8_t i = 0; i <= NUM_LEDS; i++){
		leds[i] = CRGB::Red;
		FastLED.show();
		sprintf(buf, "Led %d, Color Red", i);
		Serial.println(buf);
		delay(100);
		leds[i] = CRGB::Green;
		FastLED.show();
		sprintf(buf, "Led %d, Color Green", i);
		Serial.println(buf);
		delay(100);
		leds[i] = CRGB::Blue;
		FastLED.show();
		sprintf(buf, "Led %d, Color Blue", i);
		Serial.println(buf);
		delay(100);
		leds[i] = CRGB::Black;
		FastLED.show();
		sprintf(buf, "Led %d, off", i);
		Serial.println(buf);
	}
}

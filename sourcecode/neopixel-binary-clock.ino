#include <Wire.h>

#include <RtcDS3231.h>
#include <RtcDateTime.h>
#include <RtcUtility.h>

#include <NeoPixelAnimator.h>
#include <NeoPixelBus.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>

#include <string.h>

#include "ntp.h"
#include "timezone.h"

// WiFi
char ssid[] = "";
char pass[] = "";
WiFiUDP udp;
unsigned int udpLocalPort = 2390;

// DS3231 RTC
RtcDS3231<TwoWire> rtc(Wire);
RtcDateTime lastUpdateTime;

// Neopixel
const uint8_t PixelPin = 2;
const uint16_t PixelCount = 40;
const uint8_t ColorCount = 5;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin); // use RX pin Esp8266
HslColor Black(0.0, 0.0, 0.0);
HslColor Colors[ColorCount];
const float distance = 1.0 / (float)ColorCount;
const float step = 0.0001;

int counter = 0;

void setup()
{
	Serial.begin(115200);
	rtc.Begin();

	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, pass);
	WiFi.setAutoReconnect(true);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println();

	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	Serial.println("Starting UDP");
	udp.begin(udpLocalPort);
	Serial.print("Local port: ");
	Serial.println(udp.localPort());

	// generate starting colors
	for (uint16_t i = 0; i != ColorCount; ++i) {
		Colors[i] = HslColor(i * distance, 1.0, 0.03);
	}
	strip.Begin();
	strip.Show();

	UpdateClock();
}

void loop()
{
	RtcDateTime now = rtc.GetDateTime();
	if (now.Hour() > lastUpdateTime.Hour() || lastUpdateTime.Hour() == 23) {
		UpdateClock();
	}

	counter = (counter + 1) % 100;
	if (counter == 0)
		SerialPrintDateTime();

	NeopixelWriteTime();

	for (int i = 0; i != ColorCount; ++i) {
		if (Colors[i].H < 1 - step)
			Colors[i].H += step;
		else
			Colors[i].H = 0;
	}

	delay(10);
}

void UpdateClock()
{
	RtcDateTime now = GetNTPTime();

	now = ApplyTimezone(now);

	lastUpdateTime = now;
	rtc.SetDateTime(now);
}

void SerialPrintDateTime()
{
	RtcDateTime now = rtc.GetDateTime();

	char datestring[20];
	snprintf_P(datestring,
	           countof(datestring),
	           PSTR("%04u-%02u-%02u-%02u-%02u-%02u"),
	           now.Year(),
	           now.Month(),
	           now.Day(),
	           now.Hour(),
	           now.Minute(),
	           now.Second());
	Serial.println(datestring);
}

void NeopixelWriteNumber(uint16_t startIndex, HslColor color, uint8_t number)
{
	for (uint16_t i = 0; i != 8; ++i) {
		uint8_t mask = 1 << i;
		uint16_t pixelID = startIndex + i;
		if ((number & mask) != 0)
			strip.SetPixelColor(pixelID, color);
		else
			strip.SetPixelColor(pixelID, Black);
	}
}

void NeopixelWriteTime()
{
	RtcDateTime now = rtc.GetDateTime();
	NeopixelWriteNumber(0, Colors[0], now.Second());
	NeopixelWriteNumber(8, Colors[2], now.Minute());
	NeopixelWriteNumber(16, Colors[4], now.Hour());
	NeopixelWriteNumber(24, Colors[1], now.Day());
	NeopixelWriteNumber(32, Colors[3], now.Month());
	strip.Show();
}
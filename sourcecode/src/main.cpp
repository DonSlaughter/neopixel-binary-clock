#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <time.h>

#include <NTPClient.h>
#include <RtcDS3231.h>
#include <Timezone.h>
#include "secrets.h"
#include "main.h"


// Define NTP Client to get time
WiFiUDP ntpUDP;
//NTPClient updates normall every minute
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

RtcDS3231<TwoWire> Rtc(Wire);

void setup() {
	Serial.begin(115200);
	delay(250);
	connect_to_wifi();
	print_connection();

	timeClient.begin();
	delay(2000);
	//timeClient.update();
	Rtc.Begin();
	RTC_Update();
}

void loop(){
	//Serial.println(timeClient.getFormattedTime());
	delay(1000);
	RtcDateTime currTime = Rtc.GetDateTime();
	Serial.println(currTime);
	Serial.println(timeClient.getFormattedTime());
	RTC_Update();
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
	// Do udp NTP lookup, epoch time is unix time - subtract the 30 extra yrs (946684800UL) library expects 2000
	unsigned long epochTime = timeClient.getEpochTime() - 94664800UL;
	Rtc.SetDateTime(epochTime);
}

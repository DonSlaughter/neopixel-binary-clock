#ifndef NTP_H
#define NTP_H

const char* ntpServerName = "ptbtime1.ptb.de";
IPAddress timeServerIP;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

extern WiFiUDP udp;

void sendNTPpacket(IPAddress& address)
{
	Serial.println("sending NTP packet...");
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12]  = 49;
	packetBuffer[13]  = 0x4E;
	packetBuffer[14]  = 49;
	packetBuffer[15]  = 52;

	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:
	udp.beginPacket(address, 123); //NTP requests are to port 123
	udp.write(packetBuffer, NTP_PACKET_SIZE);
	udp.endPacket();
}

RtcDateTime GetNTPTime()
{
	WiFi.hostByName(ntpServerName, timeServerIP);
	unsigned long start = millis();
	bool success = false;
	while (true) {
		sendNTPpacket(timeServerIP);

		while (udp.parsePacket() == 0) {
			if ((millis() - start < 3000)) { // 3s timeout for NTP
				Serial.println("No packet yet");
				delay(20);
			} else {
				Serial.println("Retrying NTP");
				continue; // retry NTP after timeout
			}
		}

		Serial.println("Got answer");
		break; // got NTP
	}

	udp.read(packetBuffer, NTP_PACKET_SIZE);
	unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
	unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
	unsigned long secsSince1900 = highWord << 16 | lowWord;
	// substract 100 years in seconds, RTC is initialized with seconds from
	// 2000
	unsigned long epoch2k = secsSince1900 - 3155673600UL;

	RtcDateTime currentTime(epoch2k);
	return currentTime;
}

#endif
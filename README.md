# neopixel-binary-clock
Binary clock using Neopixels, ESP8266 and DS3231 real time clock.
I used PlatformIO for this Project. The Arduino IDE will do fine i guess.

# Known Problems
The first LED has some kind of Voltage spikes if WiFi is enabled. The trick is
to Compile this Code that the RTC gets a Timestamp and then Compile this Code
without WiFi. If someone can help with that it would be great, but it works
without WiFi.

# PCB Design
![pcb](binaryclock.jpg)

# idea from civ0
https://github.com/civ0/neopixel-binary-clock

## Timezones
Timeszones are changend with Lib 'Timezones.h'
Everything can be changed with TimeChangeRule.

## Parts
* ESP8266 WeemosD1 Mini
* Logic Level Shifter
* DS3231
* 220 Ohm Resistor
* Schottky Diode with low Voltage drop
* 2 x Capacitor 680uF
* Barrel Jack
* Screw Terminal
* 26 x WS2812B SMD Led's

## Connect to Wi-Fi
create a file named secrets.h in the folder src.
it should contain your wifi network (ssid) and your wifi password:
```c
const char *ssid = "";
const char *pass = "";
```
## Code
Code needs to be cleand up

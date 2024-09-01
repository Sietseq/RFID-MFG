# RFID MFG

## Description
This branch includes the local server used by an external device that will have a database that is synced with the cloud.\
The ESP32 uses a TTL to USB converter to communicate with the external device. 
The code is split up into 2 sections:
1. Main
2. RFID


## Arduino Setup
When in the Arduino IDE install esp32 board by Espressif Systems. When compiling be sure to use the ESP32 Dev Module.\
In the Tools menu change Partition Scheme to Huge App. This gives more space for sketchs to be stored. 

## Libraries used 
**PN532**: [Link](https://github.com/Seeed-Studio/PN532) Only the PN532 and PN532_HSU folders are needed in the ```Arduino/libraries``` directory. You will have to make the following changes in ```PN532_HSU```:\
In PN532_HSU.cpp change ```#include "PN532/PN532/PN532_debug.h"``` to ```#include "../PN532/PN532_debug.h"``` \
In PN532_HSU.h change ```#include "PN532/PN532/PN532Interface.h"``` to ```#include "../PN532/PN532Interface.h"```
<br></br>
**ArduinoJson**: Can be found through the library manager or at [Link](https://arduinojson.org/?utm_source=meta&utm_medium=library.properties)\

## Wiring
On the PN532 the following pins should be wired as following:\
**PN532** -> **ESP32**\
GND -> GND\
VCC -> 5V\
TXD -> 16\
RXD -> 17
<br></br>

**ESP32** -> **USB to TTL**\
GND -> GND\
1 -> RX\
3 -> TX\
Check the local server you may have change where you are reading data from. 


The buzzer positive terminal should be wired to pin 18 and the relay to 23.


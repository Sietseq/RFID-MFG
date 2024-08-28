# RFID MFG

## Description
This branch includes the current version at the KOA used for the gates. \
The code is split up into 3 sections:
1. Main
2. RFID
3. WiFi


## Arduino Setup
When in the Arduino IDE install esp32 board by Espressif Systems. When compiling be sure to use the ESP32 Dev Module.\
In the Tools menu change Partition Scheme to Huge App. This gives more space for sketchs to be stored. 

## Libraries used 
**PN532**: [Link](https://github.com/Seeed-Studio/PN532) Only the PN532 and PN532_HSU folders are needed in the ```Arduino/libraries``` directory. You will have to make the following changes in ```PN532_HSU```:\
In PN532_HSU.cpp change ```#include "PN532/PN532/PN532_debug.h"``` to ```#include "../PN532/PN532_debug.h"``` \
In PN532_HSU.h change ```#include "PN532/PN532/PN532Interface.h"``` to ```#include "../PN532/PN532Interface.h"```\




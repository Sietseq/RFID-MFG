# RFID MFG

## Description
This branch includes the current version at the KOA used for the gates. \
The code is split up into 4 sections:
1. Main
2. RFID
3. WiFi
4. GIFDraw


## Arduino Setup
When in the Arduino IDE install esp32 board by Espressif Systems. When compiling be sure to use the ESP32 Dev Module.\
In the Tools menu change Partition Scheme to Huge App. This gives more space for sketchs to be stored. 

## Libraries used 
**PN532**: [Link](https://github.com/Seeed-Studio/PN532) Only the PN532 and PN532_HSU folders are needed in the ```Arduino/libraries``` directory. You will have to make the following changes in ```PN532_HSU```:\
In PN532_HSU.cpp change ```#include "PN532/PN532/PN532_debug.h"``` to ```#include "../PN532/PN532_debug.h"``` \
In PN532_HSU.h change ```#include "PN532/PN532/PN532Interface.h"``` to ```#include "../PN532/PN532Interface.h"```
<br></br>
**TFT eSPI**: [Library](https://github.com/Bodmer/TFT_eSPI) This library is used to interact with the display. The following video was really great in figuring everything out: [Video](https://www.youtube.com/watch?v=rq5yPJbX_uk&t=941s). 
<br></br>
**Animations**: Gifs were used to show the animations on the screen. The following video was used as reference:[Link](https://www.youtube.com/watch?v=GrNIfwUNaaw)
<br></br>
**FastLED**: LEDs were very minimally set up but can be used. FastLED can also be downloaded through the library manager. 
## Wiring
On the PN532 the following pins should be wired as following:\
**PN532** -> **ESP32**\
GDN -> GND\
VCC -> 5V\
TXD -> 16\
RXD -> 17
***
**Display**
**Display** -> **ESP32**
VCC -> 3v3\
GND -> 3v3\
CS -> 15\
RESET -> 4\
D/c -> 2\
SDI(MOSI) -> 23\
SCK -> 18\
LED -> 3v3\

T_CLK -> 18\
T_CS -> 21\
T_DIN -> 23\
T_DO -> 19\

The buzzer positive terminal should be wired to pin 32 and the relay is not setup in this version.



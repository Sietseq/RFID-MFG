#include <PN532_HSU.h>
#include <PN532.h>


// Using UART2
PN532_HSU pn532hsu(Serial2);
PN532 nfc(pn532hsu);

// UART port
#define RXD2 16
#define TXD2 17

// Initialize RFID module
void RFID_start_up()
{
  // Start serial
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  while(!Serial2){}

  // Start NFC
  nfc.begin();

  // Get firmware version
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  nfc.setPassiveActivationRetries(0xFF);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
    
  Serial.println("Waiting for an ISO14443A card");
}

// Scan and return UID
String RFID_scan()
{
  boolean success;

  // Array of bytes
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  String strUID = "";
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Read for tag
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  // If successfull
  if (success) {
    // Loop through each byte and add to string as Hexidecimal 
    for (uint8_t i=0; i < uidLength; i++) 
    {
      Serial.println(uid[i]);

      // Could definetely be refactored here but do not want to break code
      if (uid[i] <10){
        strUID = "0" + String(uid[i], HEX) + strUID;
      }
      else if (uid[i] >= 10 && uid[i] < 16){
        strUID = "0" + String(uid[i], HEX) + strUID;
      }
      else{
        strUID = String(uid[i], HEX) + strUID;
      }
    }
  }

  // If found tag print for debug purposes
  if(strUID != ""){
    Serial.println(strUID);
  }

  // Return ID.
  return strUID;
}
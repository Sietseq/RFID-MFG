int relay = 23;
int buzzer = 18;

#include "pitches.h"
#include <Ticker.h>


// ========================= MELODY DATA ===========================
int wifiMelody[] = {
  NOTE_C5, NOTE_G5, NOTE_C6
};

int wifiNoteDurations[] = {
  4, 8, 4
};

int tagScanMelody[] = {
  NOTE_C6
};

int tagScanNoteDurations[] = {
  3
};

int successMelody[] = {
  NOTE_C5, NOTE_E5, NOTE_G5
};

int successNoteDurations[] = {
  8, 8, 4
};

int failMelody[] = {
  NOTE_G4, NOTE_E4
};

int failNoteDurations[] = {
  4, 2
};


// ======================= END MELODY DATA ===========================


/*
  Watch dog is used to restart ESP in case of any issues.
*/
Ticker secondTick;
volatile int watchDogCount = 0;
void ISRwatchdog(){
  watchDogCount++;
  if (watchDogCount == 20){
    Serial.println("The watchdog bites");
    abort();
  }
}

void setup(void) 
{ 
  // Start serial
  Serial.begin(9600);

  // Delay for serial to start
  delay(1000);

  // Set pin 23 to output
  pinMode(relay, OUTPUT);

  // Set the led pin to output
  pinMode(2, OUTPUT);

  // Wait for serial to be ready
  while(!Serial){};

  // Start up modules
  RFID_start_up();
  WiFi_start_up();

  // Start up jingle
  for (int thisNote = 0; thisNote < 3; thisNote++) {
    int noteDuration = 1000 / wifiNoteDurations[thisNote];
    tone(buzzer, wifiMelody[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzer);
  }

  // If everything successful set the built in LED
  digitalWrite(2, HIGH);

  // Setup watchdog
  secondTick.attach(1, ISRwatchdog);
}

void loop(void) 
{

  // Scan for Tag
  String result = RFID_scan();

  if (result != "")
    {

      // Scan jingle 
      for (int thisNote = 0; thisNote < 1; thisNote++) {
        int noteDuration = 1000 / tagScanNoteDurations[thisNote];
        tone(buzzer, tagScanMelody[thisNote], noteDuration);

        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(buzzer);
      }


      // Send Request
      try{
        int API_result = send_request(result);
        watchDogCount = 0;
        // Show output based on access.
        if (API_result == 200)
        {
          // Success jingle
          for (int thisNote = 0; thisNote < 3; thisNote++) {
            int noteDuration = 1000 / successNoteDurations[thisNote];
            tone(buzzer, successMelody[thisNote], noteDuration);

            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);
            noTone(buzzer);
          }

          digitalWrite(relay, HIGH);
          delay(500);
          digitalWrite(relay, LOW);
        }
        else if (API_result == 400)
        {
          // Fail jingle
          for (int thisNote = 0; thisNote < 2; thisNote++) {
            int noteDuration = 1000 / failNoteDurations[thisNote];
            tone(buzzer, failMelody[thisNote], noteDuration);

            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);
            noTone(buzzer);
          }
        }
        else{
          // Fail jingle
          for (int thisNote = 0; thisNote < 2; thisNote++) {
            int noteDuration = 1000 / failNoteDurations[thisNote];
            tone(buzzer, failMelody[thisNote], noteDuration);

            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);
            noTone(buzzer);
          }
        }
      }
      catch (String error){
        Serial.println(error);
      }

      
  }
  watchDogCount = 0;
  delay(1000);
  check_disconnect();
}

int relay = 23;
int buzzer = 18;

#include <Ticker.h>


// ========================= MELODY DATA ===========================
int wifiMelody[] = {
  523, 784, 1047
};

int wifiNoteDurations[] = {
  4, 8, 4
};

int tagScanMelody[] = {
  1047
};

int tagScanNoteDurations[] = {
  3
};

int successMelody[] = {
  523, 659, 784
};

int successNoteDurations[] = {
  8, 8, 4
};

int failMelody[] = {
  392, 330
};

int failNoteDurations[] = {
  4, 2
};

void playMelody(int notes[], int duration[], int numNotes){
  for (int thisNote = 0; thisNote < numNotes; thisNote++) {
    int noteDuration = 1000 / duration[thisNote];
    tone(buzzer, notes[thisNote], noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(buzzer);
  }
}

// ======================= END MELODY DATA ===========================


/*
  Watch dog is used to restart ESP in case of any issues.
*/
Ticker secondTick;
volatile int watchDogCount = 0;
void ISRwatchdog(){
  watchDogCount++;
  if (watchDogCount == 20){
    Serial.println(F("The watchdog bites"));
    abort();
  }
}

void setup(void) 
{ 
  // Start serial
  Serial.begin(9600);

  // Delay for serial to start
  delay(1000);

  // Just to see if the program has start
  Serial.println(F("Hello world"));

  // Set pin 23 to output
  pinMode(relay, OUTPUT);

  // Set the led pin to output
  pinMode(2, OUTPUT);

  // Wait for serial to be ready
  while(!Serial){};

  // Start up modules
  RFID_start_up();
  WiFi_start_up();

  playMelody(wifiMelody, wifiNoteDurations, 3);

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
      playMelody(tagScanMelody, tagScanNoteDurations, 1);

      // Send Request
      try{
        int API_result = send_request(result);
        watchDogCount = 0;
        Serial.println(API_result);
        // Show output based on access.
        if (API_result == 200)
        {
          playMelody(successMelody, successNoteDurations, 3);
          Serial.println(F("Access!"));
          digitalWrite(relay, HIGH);
          delay(500);
          digitalWrite(relay, LOW);
        }
        else if (API_result == 400)
        {
          playMelody(failNoteDurations, failNoteDurations, 2);
          Serial.println(F("No access!"));
        }
        else{
          Serial.print(F("Error code:"));
          // Fail jingle
          playMelody(failNoteDurations, failNoteDurations, 2);
          Serial.println(API_result);
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

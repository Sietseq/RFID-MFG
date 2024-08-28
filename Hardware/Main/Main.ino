int buzzer = 32;
int ledPin = 33;

#include <Ticker.h>
#include <AnimatedGIF.h>


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
  659, 532
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


// ========================= ANIMATION TASK ============================

volatile bool state = false;
volatile int animation_state = 0;
void AnimationTask( void * pvParameters ){
  Serial.print(F("Animation Task running on core "));
  Serial.println(xPortGetCoreID());

  for(;;){
    playAnimation(&state, &animation_state);
    state = false;
    delay(50);
  } 
}

// =================END OF ANIMATION TASK ==============================

// ======================== WATCH DOG ==================================

Ticker secondTick;
volatile int watchDogCount = 0;
void ISRwatchdog(){
  watchDogCount++;
  if (watchDogCount == 20){
    Serial.println(F("The watchdog bites"));
    abort();
  }
}

// ===================== END OF WATCHDOG ===============================

void setup(void) 
{ 
  // Start serial
  Serial.begin(9600);

  // Delay for serial to start
  delay(1000);

  // Just to see if the program has start
  Serial.println(F("Hello world"));


  // Set the led pin to output
  pinMode(2, OUTPUT);

  // Wait for serial to be ready
  while(!Serial){};

  // Start up modules
  RFID_start_up();
  WiFi_start_up();
  animation_start_up();
  
  // Play start up melody
  playMelody(wifiMelody, wifiNoteDurations, 3);

  // If everything successful set the built in LED
  digitalWrite(2, HIGH);

  // Setup watchdog
  secondTick.attach(1, ISRwatchdog);

  // set up second thread
  xTaskCreatePinnedToCore(
                    AnimationTask,   /* Task function. */
                    "AnimationTask",     /* name of task. */
                    20000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    NULL,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
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
          // When access
          animation_state = 1; 
          state = true;
          playMelody(successMelody, successNoteDurations, 3);
          Serial.println(F("Access!"));
        }
        else if (API_result == 400)
        {
          // When no access
          animation_state = 2; 
          state = true;
          playMelody(failMelody, failNoteDurations, 2);
          Serial.println(F("No access!"));
        }
        else{
          // When error
          animation_state = 2; 
          state = true;
          Serial.print(F("Error code:"));
          playMelody(failMelody, failNoteDurations, 2);
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

// Pin numbers
int ledPinGreen = 25;
int ledPinRed = 24;
int buzzer = 23;

// Will be used to either check RFID or check WiFi
boolean sentRequest = false;

void setup(void) 
{
  // Set up pins and start up modules. 
  Serial.begin(9600);
  while(!Serial){};
  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinRed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  WiFi_start_up();
  RFID_start_up();

  // Showcase start
  digitalWrite(ledPinGreen, HIGH);
  digitalWrite(ledPinRed, HIGH);
  delay(1000);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinRed, LOW);
}

void loop(void) 
{
  // If not done automatically had an issue with connecting to WiFi.
  String data = WiFi_scan();

  // If we are expecting a database response.
  if (sentRequest){
    // Show output based on access.
    if (data == "1")
    {
      digitalWrite(ledPinGreen, HIGH);
      delay(1000);
      digitalWrite(ledPinGreen, LOW);
      sentRequest = false;
    }
    else if (data == "0")
    {
      digitalWrite(ledPinRed, HIGH);
      delay(1000);
      digitalWrite(ledPinRed, LOW);
      sentRequest = false;
    }
    else if (data == "-1"){
      digitalWrite(ledPinRed, HIGH);
      delay(1000);
      digitalWrite(ledPinRed, LOW);
      sentRequest = false;
    }
    
  }

  // If not expecting, scan RFID
  else
  {
    String result = RFID_scan();
    if (result != "")
    {
      // Uncomment if want to read serial data.
      //Serial.println(result);

      // Buzzer
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(200);
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);

      // Send Request
      bool okay_request = send_request(result);
      // No connection made
      if (!okay_request){
        digitalWrite(ledPinGreen, HIGH);
        digitalWrite(ledPinRed, HIGH);
        delay(1000);
        digitalWrite(ledPinGreen, LOW);
        digitalWrite(ledPinRed, LOW);
        sentRequest = false;
      }
      sentRequest = true;
    }
  }
  

  delay(1000);
}

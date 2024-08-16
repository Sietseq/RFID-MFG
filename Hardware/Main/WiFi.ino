// Include the necessary libraries for Wi-Fi management and HTTP communication
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <IPAddress.h>
#include <HTTPClient.h>

// Define the credentials of the Wi-Fi network to connect to
const char* ssid = "****************";  // Network SSID
const char* pass = "**********";    // Network password


// Define the server to which we'll connect
String serverName = "****************************";

// Initialize the Wi-Fi client object
WiFiClient client;

void check_disconnect(){
  if ((WiFi.status() != WL_CONNECTED)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi_start_up();
  }
}

void WiFi_start_up() {
  // Attempt to connect to the defined Wi-Fi network
  Serial.print("- Attempting to connect to SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  int count = 0;

  // Wait for the connection to be established
  while ((WiFi.status() != WL_CONNECTED)) {
    count++;
    if (count > 20){
      abort();
    }
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\n- Connected to Wi-Fi network");
  printWifiStatus(); 
}

// Prints info about wifi connection
void printWifiStatus() {
  // Print network SSID
  Serial.print("- SSID: ");
  Serial.println(WiFi.SSID());

  // Print board's IP address
  Serial.print("- IP Address: ");
  Serial.println(WiFi.localIP());

  // Print signal strength
  Serial.print("- Signal strength (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
}


// This functions sends a request to the server given an ID.
int send_request(String id){
  int httpResponseCode = 0;
  //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      // Takes the website address and appends the id 
      String serverPath = serverName + id;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      http.addHeader("Authorization", "**************************");
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Send HTTP GET request
      watchDogCount = 0;
      httpResponseCode = http.GET();
      
      // If a valid http response print
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
      }

      // If not succesful
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }

    // If disconnected reconnect 
    else {
      Serial.println("WiFi Disconnected");
      Serial.println("Reconnecting to WiFi...");
    }
  return httpResponseCode;
}


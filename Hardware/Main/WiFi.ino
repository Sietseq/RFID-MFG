// Include the necessary libraries for Wi-Fi management and HTTP communication
#include "WiFiC3.h"
#include "WiFiClient.h"
#include "IPAddress.h"

// Define the credentials of the Wi-Fi network to connect to
char ssid[] = "MFG-Internal";  // Network SSID
char pass[] = "Ft9chxvxW7";  // Network password

// Define a variable for storing the status of the Wi-Fi connection
int status = WL_IDLE_STATUS;

// Define the server to which we'll connect
char server[] = "192.168.24.39";

// Initialize the Wi-Fi client object
WiFiClient client;

void WiFi_start_up() {
  // Ucoment if you are debugging
  //while (!Serial) {}

  // Check for the onboard Wi-Fi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("- Communication with Wi- Fi module failed!");
    while (true);
  }

  // Check if the Wi-Fi module's firmware is up to date
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("- Please upgrade the firmware!");
  }

  // Attempt to connect to the defined Wi-Fi network
  // Wait for the connection to be established
  while (status != WL_CONNECTED) {
    Serial.print("- Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000); 
  }

  // Print the Wi-Fi connection status
  printWifiStatus(); 
}

// Prints info about wifi connection
void printWifiStatus() {
  // Print network SSID
  Serial.print("- SSID: ");
  Serial.println(WiFi.SSID());

  // Print board's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("- IP Address: ");
  Serial.println(ip);

  // Print signal strength
  long rssi = WiFi.RSSI();
  Serial.print("- Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// This function returns a string from what the network received.
String read_response() {
  String data = "";
  while (client.available()) {

    // Actual data reception
    char c = client.read();

    // Print data to serial port
    data += c;
  }
  return data;
}

// This functions sends a request to the server given an ID.
bool send_request(String id){
  bool success = false;
  if (status == WL_CONNECTED){
    Serial.println("\n- Starting connection to server...");
    if (client.connect(server, 3000)) {
      Serial.println("- Connected to server!");
      client.println("GET /" + id + " HTTP/1.1");
      client.println("Connection: close");
      client.println();
      success = true;
    } else {
      Serial.println("- Connection failed!");
    }
  }
  return success;
}

// Will take the network data and splice it into what we are interested in.
String WiFi_scan() {
  // Read and print the server's response
  String data = read_response();
  // In case more fields are to be added use this index. 
  int index = data.lastIndexOf("permission:");
  return data.substring(index+11);
}

/*
    This sketch sends a string to a TCP server, and prints a one-line response.
    You must run a TCP server in your local network.
    For example, on Linux you can use this command: nc -v -l 3000
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#ifndef STASSID
#define STASSID "AndroidAP"
#define STAPSK  "xlah1416"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "18.197.8.98";
const uint16_t port = 5090;

ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  WiFiClient client;

  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(0.1);
    return;
  }

  delay(500);
}


void loop() {
  int i;
  int used = 0;
  int fin = 0;
  char recv_str;
  char recv_data;
  char buffer[11];
  memset(buffer, 0, 11);
  
  while (used < 10) {
    if (Serial.available() > 0) {
      recv_str = Serial.read();
     
      if (recv_str == '\n') {
        buffer[used++] = recv_data;
        recv_data = 0;
        continue;
      }
      
      recv_data += recv_data * 10 + recv_str - '0';
    }
  }
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, port)) {
//    Serial.println("connection failed");
//    Serial.println("wait 5 sec...");
    delay(0.1);
    return;
  }

  // This will send the request to the server
  client.write(buffer, 10);

  for (i = 0; i < 10; i++) {
    Serial.printf("Voltage: %d\n", buffer[i]); 
  }
  
  //read back one line from server
//  Serial.println("receiving from remote server");
  String line = client.readStringUntil('\n');
  Serial.println(line);

//  Serial.println("closing connection");
  client.stop();

  used = 0;
}

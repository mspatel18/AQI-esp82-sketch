#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
// #include <TimeLib.h>

// Replace these with your Wi-Fi credentials
const char* ssid = "<SSID>";
const char* password = "<PASSWORD>";

const char* host = "script.google.com";
const char* googleScriptId = "<YOUR_GOOGLE_SCRIPT_ID>";
const int httpsPort = 443;

const int analogPin = A0;
const int BUZZER = 6;
WiFiClientSecure client;

void setup_wifi() {
  delay(10);
  Serial.println(); 
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
}

void loop() {
  int sensorValue = analogRead(analogPin);

  Serial.print("Analog reading: ");
  Serial.println(sensorValue);

  if (WiFi.status() == WL_CONNECTED) {
    // Disable SSL certificate validation (use with caution)
    client.setInsecure();

    if (!client.connect(host, httpsPort)) {
      Serial.println("Connection failed");
      return;
    }

    // Construct the URL with date, time, and sensor value
    String url = "/macros/s/" + String(googleScriptId) + "/exec?sensor=" + String(sensorValue);

    Serial.print("Requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("Request sent");

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("Headers received");
        break;
      }
    }

    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }

    Serial.println("Closing connection");
  } else {
    Serial.println("WiFi not connected");
  }
  
  delay(1000); // Adjust delay as needed
}

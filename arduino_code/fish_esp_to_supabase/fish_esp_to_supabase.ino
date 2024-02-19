#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

// Network Credentials
const char* ssid = "Galaxy A02b3dd";
const char* password = "nevil126@";

// Supabase Credentials
String API_URL = "https://jessgwmtjqktdmtxdfdw.supabase.co";
String API_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Implc3Nnd210anFrdGRtdHhkZmR3Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3MDcyNDk2MzgsImV4cCI6MjAyMjgyNTYzOH0.DyXztLyKWY1pOWaFDAn0WY6N_DbsfjjJxZWJssuzDpI";
String TableName = "maintable";
const int httpsPort = 443;

// Sending interval 
int sendinginterval = 30; // 20 minutes

HTTPClient https;
WiFiClientSecure client;

void setup() {
  Serial.begin(115200); 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // Assuming LED is wired inversely

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW); // Indicate activity

    if (Serial.available()) {
      String data = Serial.readString(); 
       // Parse the data (if needed based on how you format it on the Arduino)
    int turbidityIndex = data.indexOf(',');
    String turbidityStr = data.substring(0, turbidityIndex);
    String distanceStr = data.substring(turbidityIndex + 1);

    int turbidity = turbidityStr.toInt();
    int distance = distanceStr.toInt();

    https.begin(client,API_URL+"/rest/v1/"+TableName);
    https.addHeader("Content-Type", "application/json");
    https.addHeader("Prefer", "return=representation");
    https.addHeader("apikey", API_KEY);
    https.addHeader("Authorization", "Bearer " + API_KEY);
    // Construct your JSON payload
    String payload = "{\"turbidity\":" + String(turbidity) + ",\"distance\":" + String(distance) + "}";
    Serial.println(payload);    //Print request response payload
    int httpCode = https.POST(payload); 
      if (httpCode > 0) {    
          Serial.printf("HTTP Response code: %d\n", httpCode);
      } else {
          Serial.println("Error on HTTP request");
      }
      https.end(); 
    } 
    digitalWrite(LED_BUILTIN, HIGH);  // Turn LED off between transmissions 
  } else {
      Serial.println("Error in WiFi connection");
  }
  delay(1000 * sendinginterval); 
}

 
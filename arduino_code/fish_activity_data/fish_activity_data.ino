#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <Ultrasonic.h>
#include <SPI.h>  // not used here, but needed to prevent a RTClib compile error
#include "RTClib.h"


/*
 * Pass as a parameter the trigger and echo pin, respectively,
 * or only the signal pin (for sensors 3 pins), like:
 * Ultrasonic ultrasonic(13);
 */
RTC_DS3231 rtc; 
Ultrasonic ultrasonic(0, 1);
int distance;

// Global constants (define if used multiple times)
// Replace with your network credentials
const char* ssid     = "Galaxy A02b3dd";
const char* password = "nevil126@";
const int sensorPin = A0;

// supabase credentials
String API_URL = "https://jessgwmtjqktdmtxdfdw.supabase.co";
String API_KEY = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Implc3Nnd210anFrdGRtdHhkZmR3Iiwicm9sZSI6ImFub24iLCJpYXQiOjE3MDcyNDk2MzgsImV4cCI6MjAyMjgyNTYzOH0.DyXztLyKWY1pOWaFDAn0WY6N_DbsfjjJxZWJssuzDpI";
String TableName = "maintable";
const int httpsPort = 443;

// Sending interval of the packets in seconds
int sendinginterval = 30; // 20 minutes
//int sendinginterval = 120; // 2 minutes

HTTPClient https;
WiFiClientSecure client;



// Function prototypes

void sendSms(const char* recipientNumber, const char* message); // Handle SMS sending
void printTurbidity(int turbidity); // Dedicated function for turbidity display



void setup() {
  rtc.begin();
  // builtIn led is used to indicate when a message is being sent
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // the builtin LED is wired backwards HIGH turns it off

  // HTTPS is used without checking credentials 
  client.setInsecure();
  Serial.begin(115200);
  
    
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Send SMS only if it's necessary (consider flag or input)
  sendSms("", "Your message here"); // Place message in function argument
}

void loop() {
   // If connected to the internet turn the Builtin led On and attempt to send a message to the database 
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_BUILTIN, LOW); // LOW turns ON
   distance = ultrasonic.read();
  
  Serial.println("Distance in CM: ");
  Serial.println(distance);
  int sensorValue = analogRead(sensorPin);
  int turbidity = map(sensorValue, 0, 750, 100, 0);

  printTurbidity(turbidity); // Call dedicated function

   // Send the a post request to the server
    https.begin(client,API_URL+"/rest/v1/"+TableName);
    https.addHeader("Content-Type", "application/json");
    https.addHeader("Prefer", "return=representation");
    https.addHeader("apikey", API_KEY);
    https.addHeader("Authorization", "Bearer " + API_KEY);
    int httpCode =https.POST("{\"turbidity\":" + String(turbidity)+ ",\"distance\":"+ String(distance) + "}");//Send the request
    String payload = https.getString(); 
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
    https.end();

    
    digitalWrite(LED_BUILTIN, HIGH); // HIGH turns off
  }else{
    Serial.println("Error in WiFi connection");
  }
  delay(1000*sendinginterval);  //wait to send the next request
  
} // Adjust delay as needed




void sendSms(const char* recipientNumber, const char* message) {
  Serial.print("AT"); // Start configuring GSM module
  delay(1000);

  Serial.println("AT+CMGF=1"); // Set GSM in text mode
  delay(1000);

  Serial.print("AT+CMGS=");
  Serial.print("\"");
  Serial.print(recipientNumber);
  Serial.println("\"");
  delay(1000);

  Serial.println(message); // Pass message as function argument
  delay(1000);

  Serial.write(26); // CTRL+Z to send text and end session
}

void printTurbidity(int turbidity) {
  delay(100);

  Serial.println("Turbidity:");
  Serial.println(turbidity);

  delay(100);
  

  if (turbidity < 20) {
    
    Serial.println(" it's CLEAR");
  } else if (turbidity < 50) {
    
    Serial.println(" it's CLOUDY");
  } else {
    Serial.println(" it's DIRTY");
    //break;
  }
}

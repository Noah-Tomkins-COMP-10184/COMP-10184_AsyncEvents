/**
 * I, Noah Tomkins, 0000790079 certify that this material is my original work.
 * No other person's work has been used without due acknowledgement.
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define PIR     D5
#define GREEN   D3
#define YELLOW  D2
#define RED     D1

const char *ssid = "";
const char *password = "";
const char *endpoint = "http://api.thingspeak.com/update?api_key=KEY&field1=1";

volatile bool connected = false;

bool redOn = false;
unsigned long redEndTime = 0;
unsigned long redOnTime = 2000;

bool motion = false;

WiFiServer server(80);

void setWifiLights() {
    digitalWrite(YELLOW, !connected);
    digitalWrite(GREEN, connected);
}

void IRAM_ATTR interrupt() {
    motion = true;
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n\nNoah Tomkins 0000790079");
    Serial.println("Async Events");
    Serial.println("");

    // Setup Pins
    pinMode(PIR, INPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(RED, OUTPUT);
    setWifiLights();
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
        delay(100);
    connected = true;
    setWifiLights();

    // Interrupt
    attachInterrupt(digitalPinToInterrupt(PIR), interrupt, RISING);
}

void loop() {
    connected = WiFi.status() == WL_CONNECTED;
    setWifiLights();

    if (motion && connected) {
        motion = false;
        Serial.println("Motion Detected");
        redEndTime = millis() + redOnTime;

        WiFiClient client;
        HTTPClient http;

        http.begin(client, endpoint);
        int code = http.GET();
        
        if (code != 200)
            Serial.println("Failed to send request to server!");
    }

    redOn = millis() <= redEndTime;
    digitalWrite(RED, redOn);  
}
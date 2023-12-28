#include <Arduino.h>
#include "WiFi.h"
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include "ArduinoOTA.h"
#include <Adafruit_NeoPixel.h>

#include <Credentials.h>

// WiFi network credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PW;

// Blink LED
const int led = 2;                // ESP32 Pin to which onboard LED is connected
unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = 1000;       // interval at which to blink (milliseconds)
int ledState = LOW;               // ledState used to set the LED

// Neopixels
#define PIN_NEO_PIXEL 16 // The ESP32 pin GPIO16 connected to NeoPixel
#define NUM_PIXELS 4     // The number of LEDs (pixels) on NeoPixel LED strip
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRBW + NEO_KHZ800);

// put function declarations here:
int myFunction(int, int);

void setup()
{
  // Init serial interface
  Serial.begin(115200);
  Serial.println("Booting");

  // Connect WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println('.');
    delay(100);
  }
  Serial.println("WiFi connected!");

  // OTA
  ArduinoOTA.setHostname("christmas-crib");
  // ArduinoOTA.setPassword("jesus");
  ArduinoOTA
      .onStart([]()
               {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); })
      .onEnd([]()
             { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total)
                  { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
      .onError([](ota_error_t error)
               {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Neopixels
  NeoPixel.begin(); // initialize NeoPixel strip object (REQUIRED)

  // Real code here
  // int result = myFunction(2, 3);

  // Blink LED
  pinMode(led, OUTPUT);
}

void loop()
{
  // OTA
  ArduinoOTA.handle();

  // loop to blink without delay
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    Serial.println("Toggle LED");
    Serial.println(currentMillis);
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    ledState = not(ledState);
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);

    NeoPixel.clear();
    NeoPixel.setPixelColor(0, NeoPixel.Color(10, 0, 0, 0));
    NeoPixel.setPixelColor(1, NeoPixel.Color(0, 10, 0, 0));
    NeoPixel.setPixelColor(2, NeoPixel.Color(0, 0, 10, 0));
    NeoPixel.setPixelColor(3, NeoPixel.Color(0, 0, 0, 10));
    NeoPixel.show();
  }
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}
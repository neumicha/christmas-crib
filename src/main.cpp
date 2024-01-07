#include <Arduino.h>
#include <stdlib.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include "ArduinoOTA.h"
#include <Adafruit_NeoPixel.h>

#include <Credentials.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
#include <string>
#include "Audio.h"
#include <AccelStepper.h>

#include <Preferences.h>
#include "CCSettings.h"

// WiFi network credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PW;

// Webserver
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
String message = "";
String sliderValue1 = "0";
String sliderValue2 = "0";
String sliderValue3 = "0";
String color1value = "0";
byte r, g, b;
int dutyCycle1;
int dutyCycle2;
int dutyCycle3;
const int resolution = 8;
// Json Variable to Hold Slider Values
JSONVar sliderValues;
CCSettings ccSettings;

// Blink LED
const int led = 2;                // ESP32 Pin to which onboard LED is connected
unsigned long previousMillis = 0; // will store last time LED was updated
const long interval = 1000;       // interval at which to blink (milliseconds)
int ledState = LOW;               // ledState used to set the LED

// Neopixels
#define PIN_NEO_PIXEL 16 // The ESP32 pin GPIO16 connected to NeoPixel
#define NUM_PIXELS 4     // The number of LEDs (pixels) on NeoPixel LED strip
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRBW + NEO_KHZ800);

// Audio
// I2S Connection
#define I2S_DOUT 18
#define I2S_BCLK 19
#define I2S_LRC 21
// Audio object
Audio audio;
// Volume
int audioVolume = 5;

// Stepper Motor Driver ULN2003
#define STEPPER1_IN1 25
#define STEPPER1_IN2 26
#define STEPPER1_IN3 27
#define STEPPER1_IN4 14
const int stepper1max = 10;
const int stepper1sps = 2048 / 60; // steps per second for one revolution
int stepper1speed;
AccelStepper stepper1(AccelStepper::FULL4WIRE, STEPPER1_IN1, STEPPER1_IN3, STEPPER1_IN2, STEPPER1_IN4);

Preferences preferences;

// Get Slider Values
String getSliderValues()
{
  sliderValues["sliderValue1"] = String(sliderValue1);
  sliderValues["sliderValue2"] = String(sliderValue2);
  sliderValues["sliderValue3"] = String(sliderValue3);
  String jsonString = JSON.stringify(sliderValues);
  return jsonString;
}

// Initialize SPIFFS
void initFS()
{
  // SPIFFS.begin(true); // TODO: Is this sometimes needed? https://github.com/espressif/arduino-esp32/issues/638
  if (!SPIFFS.begin())
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  else
  {
    Serial.println("SPIFFS mounted successfully");
  }
}

void notifyClients(String sliderValues)
{
  ws.textAll(sliderValues);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    message = (char *)data;
    Serial.print("Handling websocket message");
    const char ind_preset = message.indexOf("p=");
    if (ind_preset == 0)
    { // new format with presets
      long preset = message.substring(ind_preset + 2, ind_preset + 3).toInt();
      String keyAndValue = message.substring(ind_preset + 4); // Part after "&"
      const char ind_equals = keyAndValue.indexOf("=");
      String key = keyAndValue.substring(0, ind_equals - 1);
      uint8_t keyIndex = keyAndValue.substring(ind_equals - 1, ind_equals).toInt();
      String value = keyAndValue.substring(ind_equals + 1);
      // TODO Pay attention when receiving that fancy start/stop hack
      Serial.println("Received the following command:");
      Serial.print("Preset ");
      Serial.println(preset);
      Serial.print("Key ");
      Serial.println(key);
      Serial.print("Value ");
      Serial.println(value);
      if (key == "lRgb")
      {
        Serial.print("Handle Light(RGB) with Index ");
        Serial.println(keyIndex);
        ccSettings.lRgb[keyIndex] = value.substring(1);
        // TODO Implement this
      }
      // TODO Implement other cases (white, motors, ...) and update those things after change
    }
    if (message.indexOf("1s") >= 0)
    {
      sliderValue1 = message.substring(2);
      dutyCycle1 = map(sliderValue1.toInt(), 0, 100, 0, 255);
      Serial.println(dutyCycle1);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("2s") >= 0)
    {
      sliderValue2 = message.substring(2);
      dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 255);
      Serial.println(dutyCycle2);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("3s") >= 0)
    {
      sliderValue3 = message.substring(2);
      dutyCycle3 = map(sliderValue3.toInt(), 0, 100, 0, 255);
      Serial.println(dutyCycle3);
      Serial.print(getSliderValues());
      notifyClients(getSliderValues());
    }
    if (message.indexOf("color1") >= 0) // TODO Make this prettier
    {
      color1value = message.substring(8); // TODO Make this prettier (length of color1 + 2 for =#)
      Serial.println("Color1 value set: " + color1value);
      notifyClients(getSliderValues()); // TODO Fix this
    }
    if (message.indexOf("sound1play") >= 0) // TODO Prettify this
    {
      Serial.println("Play sound1");
      // Open music file
      // audio.connecttohost("http://stream.antennethueringen.de/live/aac-64/stream.antennethueringen.de/");
      audio.connecttoFS(SPIFFS, "/audio/test.mp3");
    }
    // sound1volume
    if (message.indexOf("sound1volume") >= 0)
    {
      Serial.println("Set volume of sound1");
      audio.setVolume(message.substring(13).toInt());
    }
    // stepper1speed
    if (message.indexOf("stepper1") >= 0)
    {
      Serial.print("Setting speed of stepper1 to: ");
      Serial.println(message.substring(9).toInt());
      stepper1speed = message.substring(9).toInt();
      stepper1.setSpeed(stepper1sps * stepper1speed);
      // TODO Check if speed is over max!
    }
    /*
    // Save preferences
    if (message.indexOf("save") >= 0)
    {
      Serial.print("Saving...");
      preferences.putString();
      jsonString.
      JSONVar
    }*/
    if (strcmp((char *)data, "getValues") == 0)
    {
      notifyClients(getSliderValues());
    }
  }
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup()
{
  // Init serial interface
  Serial.begin(115200);
  Serial.println("Booting");
  Serial.print("Flash: ");
  Serial.println(ESP.getFlashChipSize());

  Serial.println("Loading preferences");

  // TODO Really load them
  // And close Perferences afterwards...
  Serial.println("Settings:");
  Serial.println(ccSettings.toString());

  ccSettings.lRgb[0] = "abc";
  ccSettings.lWhite[0] = 50;

  initFS();

  // Connect WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi: ");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
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

  // Webserver
  initWebSocket();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Serial.println("ESP32 Web Server: Send index.html");
              request->send(SPIFFS, "/webserver/index.html", "text/html"); });
  server.serveStatic("/", SPIFFS, "/webserver/");

  // 404 and 405 error handler
  server.onNotFound([](AsyncWebServerRequest *request)
                    {
    if (request->method() == HTTP_GET) {
      // Handle 404 Not Found error
      Serial.println("Web Server: Not Found");
      request->send(404, "text/html", "Error 404");
    } else {
      // Handle 405 Method Not Allowed error
      Serial.println("Web Server: Method Not Allowed");
      request->send(405, "text/html", "Error 405");
    } });

  server.begin();

  // Neopixels
  NeoPixel.begin(); // initialize NeoPixel strip object (REQUIRED)

  // Audio
  // Setup I2S
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  // Set Volume
  audio.setVolume(audioVolume);

  // Real code here
  // int result = myFunction(2, 3);

  // Stepper
  stepper1.setMaxSpeed(stepper1sps * stepper1max);
  stepper1.setSpeed(stepper1sps * stepper1speed);

  // Preferences
  preferences.begin("c-crib", false);
  preferences.putString("source1", "int:hallo");

  // Blink LED
  pinMode(led, OUTPUT);
}

void getRGB(const char *text, byte &r, byte &g, byte &b)
{
  long l = strtol(text + 1, NULL, 16);
  r = l >> 16;
  g = l >> 8;
  b = l;
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

    // Neopixel
    NeoPixel.clear();
    NeoPixel.setPixelColor(0, NeoPixel.Color(127, 0, 0, 0));
    Serial.println("Color1Val: " + color1value);
    getRGB(color1value.c_str(), r, g, b);
    String red = color1value.substring(0, 2);
    String green = color1value.substring(2, 4);
    String blue = color1value.substring(4, 6);
    Serial.print("red: ");
    Serial.println(r);
    Serial.print("green: ");
    Serial.println(g);
    Serial.print("blue: ");
    Serial.println(b);
    // Serial.println("red: " + red);
    // Serial.println("green: " + green);
    // Serial.println("blue: " + blue);
    // uint8_t red2 = stoi(red.c_str(), red.c_str()+2, 16);
    // uint8_t green2 = strtol(green.c_str(), &blue.c_str(), 16);
    // uint8_t blue2 = strtol(blue.c_str(), NULL, 16);
    // Serial.println("red2: " + red2);
    // Serial.println("green2: " + green2);

    NeoPixel.setPixelColor(1, NeoPixel.Color(r, g, b, 0));
    NeoPixel.setPixelColor(2, NeoPixel.Color(0, 0, 0, 0));
    NeoPixel.setPixelColor(3, NeoPixel.Color(0, 0, 0, 0));
    NeoPixel.setBrightness(dutyCycle1);
    NeoPixel.show();

    Serial.print("Preference: ");
    Serial.println(preferences.getString("source1"));
    Serial.println("Settings:");
    Serial.println(ccSettings.toString());
  }

  // Webserver
  ws.cleanupClients();

  // Audio
  audio.loop();

  // Stepper
  stepper1.runSpeed();
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}

// Audio stuff (optional)
void audio_info(const char *info)
{
  Serial.print("info        ");
  Serial.println(info);
}
void audio_id3data(const char *info)
{ // id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ // end of file
  Serial.print("eof_mp3     ");
  Serial.println(info);
}
void audio_showstation(const char *info)
{
  Serial.print("station     ");
  Serial.println(info);
}
void audio_showstreaminfo(const char *info)
{
  Serial.print("streaminfo  ");
  Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
  Serial.print("streamtitle ");
  Serial.println(info);
}
void audio_bitrate(const char *info)
{
  Serial.print("bitrate     ");
  Serial.println(info);
}
void audio_commercial(const char *info)
{ // duration in sec
  Serial.print("commercial  ");
  Serial.println(info);
}
void audio_icyurl(const char *info)
{ // homepage
  Serial.print("icyurl      ");
  Serial.println(info);
}
void audio_lasthost(const char *info)
{ // stream URL played
  Serial.print("lasthost    ");
  Serial.println(info);
}
void audio_eof_speech(const char *info)
{
  Serial.print("eof_speech  ");
  Serial.println(info);
}
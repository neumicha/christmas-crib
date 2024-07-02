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
// #include <Arduino_JSON.h>
#include <string>
#include <AccelStepper.h>

#include <Preferences.h>
#include "CCSettings.h"
#include <ArduinoJson.h>

#include "AudioController.h"

// To erase nvs partition
#include <nvs_flash.h>

// WiFi network credentials
constexpr char *ssid = WIFI_SSID;
constexpr char *password = WIFI_PW;

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
// JSONVar sliderValues;
CCSettings ccSettings[2];

// Blink LED
constexpr int led = 2;            // ESP32 Pin to which onboard LED is connected
unsigned long previousMillis = 0; // will store last time LED was updated
constexpr long interval = 10000;  // interval at which to blink (milliseconds)
int ledState = LOW;               // ledState used to set the LED

// Neopixels
constexpr int PIN_NEO_PIXEL = 16; // The ESP32 pin GPIO16 connected to NeoPixel
constexpr int NUM_PIXELS = 5;     // The number of LEDs (pixels) on NeoPixel LED strip
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRBW + NEO_KHZ800);

// Audio
// I2S Connection
constexpr int I2S_DOUT = 18;
constexpr int I2S_BCLK = 19;
constexpr int I2S_LRC = 21;
// Volume
int audioVolume = 5;
// Audio object
AudioController audioController;

// Stepper Motor Driver ULN2003
constexpr int STEPPER1_IN1 = 25;
constexpr int STEPPER1_IN2 = 26;
constexpr int STEPPER1_IN3 = 27;
constexpr int STEPPER1_IN4 = 14;
constexpr int STEPPER_MAX = 10;
#define STEPPER_SPS ((2048 * 16) / 60) // steps per second for one revolution
AccelStepper motors[1] = {AccelStepper(AccelStepper::FULL4WIRE, STEPPER1_IN1, STEPPER1_IN3, STEPPER1_IN2, STEPPER1_IN4)};

Preferences preferences;

// Get Slider Values
String getSliderValues()
{
  // sliderValues["sliderValue1"] = String(sliderValue1);
  // sliderValues["sliderValue2"] = String(sliderValue2);
  // sliderValues["sliderValue3"] = String(sliderValue3);
  // String jsonString = JSON.stringify(sliderValues);
  // return jsonString;
  return "";
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

void getRGB(const char *text, byte &r, byte &g, byte &b)
{
  long l = strtol(text + 1, NULL, 16);
  r = l >> 16;
  g = l >> 8;
  b = l;
}

void updateLights(CCSettings *settings)
{
  NeoPixel.clear();
  for (int i = 0; i < 4; i++) // TODO Set boundary to define
  {
    getRGB(settings->stringSettings["lRgb"][i].c_str(), r, g, b);
    NeoPixel.setPixelColor(i, NeoPixel.Color(r, g, b, settings->intSettings["lWhite"][i]));
  }
  NeoPixel.setPixelColor(4, NeoPixel.Color(0, 0, 0, 0));
  NeoPixel.show();
}

void updateMotors(CCSettings *settings)
{
  for (int i = 0; i < 1; i++) // TODO Set boundary to define
  {
    motors[i].setSpeed(STEPPER_SPS * min(abs(settings->intSettings["mSpeed"][i]), STEPPER_MAX));
    Serial.print("Setting motor speed to ");
    Serial.println(STEPPER_SPS * min(abs(settings->intSettings["mSpeed"][i]), STEPPER_MAX));
  }
}

void updateSound(CCSettings *settings)
{
  for (int i = 0; i < 1; i++) // TODO Set boundary to define
  // TODO Put audioController into array
  {
    if (settings->stringSettings["sSource"][i] != "" && settings->boolSettings["sState"][i] && settings->intSettings["sVolume"][i] > 0)
    {
      if (settings->stringSettings["sSource"][i].indexOf("http") == 0)
      {
        audioController.connecttohost(settings->stringSettings["sSource"][i]);
      }
      else
      {
        audioController.connecttoFS(SPIFFS, settings->stringSettings["sSource"][i]);
      }
      audioController.setVolume(settings->intSettings["sVolume"][i]);
    }
    else
    {
      audioController.setVolume(0);
      audioController.stopSong();
    }
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    message = reinterpret_cast<char *>(data);
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
      Serial.print("KeyIndex ");
      Serial.println(keyIndex);
      Serial.print("Value ");
      Serial.println(value);
      Serial.print("Full message ");
      Serial.println(message);
      String ints[] = {"lWhite", "mSpeed", "aType", "aParam", "sVolume"};
      String strings[] = {"lRgb", "sSource"};
      String bools[] = {"sState"};
      if (std::find(std::begin(ints), std::end(ints), key) != std::end(ints))
      {
        ccSettings[0].intSettings[key][keyIndex] = value.toInt();
      }
      if (std::find(std::begin(strings), std::end(strings), key) != std::end(strings))
      {
        ccSettings[0].stringSettings[key][keyIndex] = value;
      }
      if (std::find(std::begin(bools), std::end(bools), key) != std::end(bools))
      {
        ccSettings[0].boolSettings[key][keyIndex] = (value.indexOf("1") == 0); // Map everything starting with "1" to true
      }

      if (key.indexOf("l") == 0)
      { // Lights
        updateLights(&ccSettings[0]);
      }
      else if (key.indexOf("m") == 0)
      { // Motors
        updateMotors(&ccSettings[0]);
      }
      else if (key.indexOf("s") == 0)
      { // Motors
        updateSound(&ccSettings[0]);
      }
      // TODO Implement other cases (white, motors, ...) and update those things after change
    }
    // if (message.indexOf("1s") >= 0)
    // {
    //   sliderValue1 = message.substring(2);
    //   dutyCycle1 = map(sliderValue1.toInt(), 0, 100, 0, 255);
    //   Serial.println(dutyCycle1);
    //   Serial.print(getSliderValues());
    //   notifyClients(getSliderValues());
    // }
    // if (message.indexOf("2s") >= 0)
    // {
    //   sliderValue2 = message.substring(2);
    //   dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 255);
    //   Serial.println(dutyCycle2);
    //   Serial.print(getSliderValues());
    //   notifyClients(getSliderValues());
    // }
    // if (message.indexOf("3s") >= 0)
    // {
    //   sliderValue3 = message.substring(2);
    //   dutyCycle3 = map(sliderValue3.toInt(), 0, 100, 0, 255);
    //   Serial.println(dutyCycle3);
    //   Serial.print(getSliderValues());
    //   notifyClients(getSliderValues());
    // }

    // Save preferences
    if (message.indexOf("pSave") >= 0)
    {
      Serial.print("Saving...");
      // preferences.putString(0, static_cast<char *>(ccSettings[0].toJSON()));
      String saveString;
      serializeJson(ccSettings[0].toJSON(), saveString);
      preferences.putString("settings", saveString);
    }
    else if (message.indexOf("pLoad") >= 0)
    {
      Serial.print("Loading...");
      String loadString = preferences.getString("settings");
      if (!loadString.isEmpty())
      {
        JsonDocument settings;
        deserializeJson(settings, loadString);
        Serial.println("Settings after loading:");
        // serializeJsonPretty(settings, Serial);
        // ccSettings[0].intSettings["sVolume"][0] = settings["sVolume"];
      }
    }

    // ccSettings[0].intSettings["lRgb"][0] = settings["lRgb"];
  }
  else if (message.indexOf("pReset") >= 0)
  {
    Serial.print("Resetting...");
    nvs_flash_erase(); // erase the NVS partition and...
    nvs_flash_init();  // initialize the NVS partition.
  }
  if (strcmp(reinterpret_cast<char *>(data), "getValues") == 0)
  {
    notifyClients(getSliderValues());
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
  Serial.println("Settings on boot:");
  serializeJsonPretty(ccSettings[0].toJSON(), Serial);
  // Serial.println("1:");
  // Serial.println(ccSettings[1].toString());

  // ccSettings[0].lRgb[0] = "abc";
  // ccSettings[1].lWhite[0] = 50;

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
  NeoPixel.begin();            // initialize NeoPixel strip object (REQUIRED)
  NeoPixel.setBrightness(255); // Max brightness

  // Audio
  audioController.setup(I2S_BCLK, I2S_LRC, I2S_DOUT, audioVolume);
  audioController.connecttohost("http://stream.antennethueringen.de/live/aac-64/stream.antennethueringen.de/");

  // Motors
  for (int i = 0; i < 1; i++)
  {
    motors[i].setMaxSpeed(STEPPER_SPS * STEPPER_MAX);
    motors[i].setSpeed(0);
  }

  // Preferences
  preferences.begin("c-crib", false);
  preferences.putString("source1", "int:hallo");

  // Blink LED
  pinMode(led, OUTPUT);

  // Test of JSON
  JsonDocument doc;
  doc["ex1"] = "abcdef";
  doc["ex2"] = 123;
  JsonArray data = doc["ex3"].to<JsonArray>();
  data.add("abc");
  data.add(456);
  Serial.println("");
  serializeJsonPretty(doc, Serial);
}

void loop()
{
  // OTA
  ArduinoOTA.handle();

  // loop to blink without delay
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  { // save the last time you blinked the LED
    previousMillis = currentMillis;
    // if the LED is off turn it on and vice-versa:
    ledState = not(ledState);
    // set the LED with the ledState of the variable:
    digitalWrite(led, ledState);

    Serial.print("Preference: ");
    Serial.println("Settings 0:");
    serializeJsonPretty(ccSettings[0].toJSON(), Serial);
  }

  // Webserver
  ws.cleanupClients();

  // Audio
  audioController.loop();

  // Stepper
  for (int i = 0; i < 1; i++)
  {
    motors[i].runSpeed();
  }
}

// put function definitions here:
int myFunction(int x, int y)
{
  return x + y;
}
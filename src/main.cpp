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
#include <string>
#include <AccelStepper.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <nvs_flash.h>
#include "CCSettings.h"
#include "AudioController.h"
#include "FireController.h"

// WiFi network credentials
constexpr char *ssid = WIFI_SSID;
constexpr char *password = WIFI_PW;

// Webserver
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
String message = "";

// Settings (currently only one supported, however)
CCSettings ccSettings[1];

// Blink LED
constexpr int led = 2;            // ESP32 Pin to which onboard LED is connected
unsigned long previousMillis = 0; // will store last time LED was updated
constexpr long interval = 10000;  // interval at which to blink (milliseconds)
int ledState = LOW;               // ledState used to set the LED

// Neopixels
byte r, g, b;
constexpr int PIN_NEO_PIXEL = 16; // The ESP32 pin GPIO16 connected to NeoPixel
constexpr int NUM_PIXELS = 8;     // The number of LEDs (pixels) on NeoPixel LED strip
Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRBW + NEO_KHZ800);
FireController fireController(&NeoPixel, LIGHTS, NUM_PIXELS - 1); // Rest of Neopixels is fire

// Audio
// I2S Connection
constexpr int I2S_DOUT = 18;
constexpr int I2S_BCLK = 19;
constexpr int I2S_LRC = 21;
// Volume
int audioVolume = 0;
// Audio object
AudioController audioController;

// Stepper Motor Driver ULN2003
constexpr int STEPPER1_IN1 = 25;
constexpr int STEPPER1_IN2 = 26;
constexpr int STEPPER1_IN3 = 27;
constexpr int STEPPER1_IN4 = 14;
constexpr int STEPPER_MAX = 10;
constexpr float STEPPER_SPS = 2048 / 60; // steps per second for one revolution
AccelStepper motors[1] = {AccelStepper(AccelStepper::FULL4WIRE, STEPPER1_IN1, STEPPER1_IN3, STEPPER1_IN2, STEPPER1_IN4)};

Preferences preferences;

// Use both cores
TaskHandle_t Task1;
TaskHandle_t Task2;

// Get Slider Values
String getSliderValues()
{
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

void notifyClients(CCSettings *settings)
{
  String str;
  serializeJson(settings->toJSON(), str);
  ws.textAll(str);
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
  // NeoPixel.clear();
  for (int i = 0; i < LIGHTS; i++)
  {
    getRGB(settings->stringSettings["lRgb"][i].c_str(), r, g, b);
    NeoPixel.setPixelColor(i, NeoPixel.Color(NeoPixel.gamma8(r), NeoPixel.gamma8(g), NeoPixel.gamma8(b), NeoPixel.gamma8(settings->intSettings["lWhite"][i])));
  }
  NeoPixel.show();
}

void updateMotors(CCSettings *settings)
{
  for (int i = 0; i < MOTORS; i++)
  {
    motors[i]
        .setSpeed(STEPPER_SPS * min(abs(settings->intSettings["mSpeed"][i]), STEPPER_MAX));
  }
}

void updateSound(CCSettings *settings)
{
  for (int i = 0; i < SOUNDS; i++)
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

void showSettings()
{
  // Update everything
  updateLights(&ccSettings[0]);
  updateMotors(&ccSettings[0]);
  updateSound(&ccSettings[0]);
  notifyClients(&ccSettings[0]);
}

void loadSettings()
{
  Serial.print("Loading...");
  String loadString = preferences.getString("settings");
  if (!loadString.isEmpty())
  {
    JsonDocument settings;
    deserializeJson(settings, loadString);
    ccSettings[0] = *new CCSettings(loadString);
    Serial.println("Settings after loading:");
    serializeJsonPretty(ccSettings[0].toJSON(), Serial);
    showSettings();
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
    {
      long preset = message.substring(ind_preset + 2, ind_preset + 3).toInt();
      String keyAndValue = message.substring(ind_preset + 4); // Part after "&"
      const char ind_equals = keyAndValue.indexOf("=");
      String key = keyAndValue.substring(0, ind_equals - 1);
      uint8_t keyIndex = keyAndValue.substring(ind_equals - 1, ind_equals).toInt();
      String value = keyAndValue.substring(ind_equals + 1);
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
      String ints[] = {"lWhite", "mSpeed", "aType", "sVolume"};
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
      { // Sound
        updateSound(&ccSettings[0]);
      }
      notifyClients(&ccSettings[0]);
    }

    // Save preferences
    if (message.indexOf("pSave") >= 0)
    {
      Serial.print("Saving...");
      String saveString;
      serializeJson(ccSettings[0].toJSON(), saveString);
      preferences.putString("settings", saveString);
    }
    else if (message.indexOf("pLoad") >= 0)
    {
      loadSettings();
    }
    else if (message.indexOf("pReset") >= 0)
    {
      Serial.print("Resetting...");
      nvs_flash_erase(); // erase the NVS partition and...
      nvs_flash_init();  // initialize the NVS partition.
    }
  }
  if (strcmp(reinterpret_cast<char *>(data), "getValues") == 0)
  {
    notifyClients(&ccSettings[0]);
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

// See https://randomnerdtutorials.com/esp32-dual-core-arduino-ide/ for multicore usage
// Core 0
void Task2code(void *pvParameters)
{
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  for (;;)
  {
    // OTA
    ArduinoOTA.handle();

    // Stepper
    for (int i = 0; i < MOTORS; i++)
    {
      motors[i].runSpeed();
    }

    // Fire
    fireController.animate(ccSettings[0].intSettings["aType"][0]);

    // Webserver
    ws.cleanupClients();

    // loop to blink
    if (millis() - previousMillis > interval)
    { // save the last time you blinked the LED
      previousMillis = millis();
      // if the LED is off turn it on and vice-versa:
      ledState = not(ledState);
      // set the LED with the ledState of the variable:
      digitalWrite(led, ledState);

      Serial.print("Preferences: ");
      serializeJsonPretty(ccSettings[0].toJSON(), Serial);
    }
  }
}

// Core 1
void Task1code(void *pvParameters)
{
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  for (;;)
  {
    // Audio
    audioController.loop();
  }
}

void setup()
{
  // Init serial interface
  Serial.begin(115200);
  Serial.println("Booting");
  Serial.print("Setup running on core ");
  Serial.println(xPortGetCoreID());

  Serial.print("Flash: ");
  Serial.println(ESP.getFlashChipSize());

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

  // Motors
  for (int i = 0; i < MOTORS; i++)
  {
    motors[i].setMaxSpeed(STEPPER_SPS * STEPPER_MAX);
  }

  // Blink LED
  pinMode(led, OUTPUT);

  // Preferences
  preferences.begin("c-crib", false);
  Serial.println("Loading preferences");
  loadSettings();
  Serial.println("Settings on boot:");
  serializeJsonPretty(ccSettings[0].toJSON(), Serial);

  // Creating tasks for both cores
  xTaskCreatePinnedToCore(
      Task1code, /* Task function. */
      "Task1",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      1,         /* priority of the task */
      &Task1,    /* Task handle to keep track of created task */
      0);        /* pin task to core 0 */
  delay(500);

  // create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
      Task2code, /* Task function. */
      "Task2",   /* name of task. */
      10000,     /* Stack size of task */
      NULL,      /* parameter of the task */
      1,         /* priority of the task */
      &Task2,    /* Task handle to keep track of created task */
      1);        /* pin task to core 1 */
  delay(500);
}

void loop()
{
}
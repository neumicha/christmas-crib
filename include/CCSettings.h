#ifndef CCSETTINGS_H
#define CCSETTINGS_H

constexpr int LIGHTS = 5;
constexpr int MOTORS = 1;
constexpr int ANIMATIONS = 1;
constexpr int SOUNDS = 1;

#include "WString.h"
#include "ostringstream.cpp"
#include <ArduinoJson.h>
#include <vector>
#include <map>

class CCSettings
{
public:
    CCSettings();
    CCSettings(String preferences);

    std::map<String, std::vector<int>> intSettings{
        {"lWhite", std::vector<int>{std::vector<int>(LIGHTS, 0)}},
        {"mSpeed", std::vector<int>{std::vector<int>(MOTORS, 0)}},
        {"aType", std::vector<int>{std::vector<int>(ANIMATIONS, 0)}},
        {"sVolume", std::vector<int>{std::vector<int>(SOUNDS, 0)}},
    };
    std::map<String, std::vector<String>> stringSettings{
        {"lRgb", std::vector<String>{std::vector<String>(LIGHTS, "#000000")}},
        {"sSource", std::vector<String>{std::vector<String>(SOUNDS, "https://stream.antenne.de/antenne/stream/mp3")}},
    };
    std::map<String, std::vector<bool>> boolSettings{
        {"sState", std::vector<bool>{std::vector<bool>(SOUNDS, false)}},
    };

    JsonDocument toJSON();
};

#endif
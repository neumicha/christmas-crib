#ifndef CCSETTINGS_H
#define CCSETTINGS_H

#define L_RGB 4
#define L_WHITE 4
#define M_SPEED 1
#define A_TYPE 2
#define A_PARAM 2
#define S_SOURCE 1
#define S_VOLUME 1
#define S_STATE 1

#include "WString.h"
#include "ostringstream.cpp"
#include <ArduinoJson.h>
#include <vector>
#include <map>

struct CCSettings
{
    // std::vector<int> lWhite{std::vector<int>(L_WHITE, 0)};
    std::map<String, std::vector<int>> intSettings{
        {"lWhite", std::vector<int>{std::vector<int>(L_WHITE, 0)}},
        {"mSpeed", std::vector<int>{std::vector<int>(M_SPEED, 0)}},
        {"aType", std::vector<int>{std::vector<int>(A_TYPE, 0)}},
        {"aParam", std::vector<int>{std::vector<int>(A_PARAM, 0)}},
        {"sVolume", std::vector<int>{std::vector<int>(S_VOLUME, 0)}},
    };
    std::map<String, std::vector<String>> stringSettings{
        {"lRgb", std::vector<String>{std::vector<String>(L_RGB, "#000000")}},
        {"sSource", std::vector<String>{std::vector<String>(S_SOURCE, "")}},
    };
    std::map<String, std::vector<bool>> boolSettings{
        {"sState", std::vector<bool>{std::vector<bool>(S_STATE, false)}},
    };

    // Lights
    // String lRgb[L_RGB] = {"000000", "000000", "000000", "000000"};
    // settings.insert({"lWhite", lWhite});
    //  Motors
    // uint8_t mSpeed[M_SPEED] = {0};
    // // Animations
    // uint8_t aType[A_TYPE] = {0, 0};
    // uint8_t aParam[A_PARAM] = {0, 0};
    // // Sounds
    // String sSource[S_SOURCE] = {""};
    // uint8_t sVolume[S_VOLUME] = {3};
    // bool sState[S_STATE] = {false};

    String toString()
    {
        // ard::ostringstream oss;
        // oss << "Lights (rgb): ";
        // for (int i = 0; i < L_RGB; i++)
        // {
        //     oss << lRgb[i].c_str() << " | ";
        // }
        // oss << '\n'
        //     << "Lights (white): ";
        // for (int i = 0; i < L_WHITE; i++)
        // {
        //     // oss << std::to_string(lWhite[i]) << " | ";
        // }
        // oss << '\n'
        //     << "Motors (speed): ";
        // for (int i = 0; i < M_SPEED; i++)
        // {
        //     oss << std::to_string(mSpeed[i]) << " | ";
        // }
        // oss << '\n'
        //     << "Animations (type): ";
        // for (int i = 0; i < A_TYPE; i++)
        // {
        //     oss << std::to_string(aType[i]) << " | ";
        // }
        // oss << '\n'
        //     << "Animations (param): ";
        // for (int i = 0; i < A_PARAM; i++)
        // {
        //     oss << std::to_string(aParam[i]) << " | ";
        // }
        // oss << '\n'
        //     << "Sounds (source): ";
        // for (int i = 0; i < S_SOURCE; i++)
        // {
        //     oss << sSource[i].c_str() << " | ";
        // }
        // oss << '\n'
        //     << "Sounds (volume): ";
        // for (int i = 0; i < S_VOLUME; i++)
        // {
        //     oss << std::to_string(sVolume[i]) << " | ";
        // }
        // oss << '\n'
        //     << "Sounds (state): ";
        // for (int i = 0; i < S_STATE; i++)
        // {
        //     oss << std::to_string(sState[i]) << " | ";
        // }
        // return String(oss.str().c_str());
        // return this->toJSON();
        return "";
    }

    JsonDocument toJSON()
    {
        JsonDocument doc;

        for (const auto &[setting_key, setting_vector] : stringSettings)
        {
            JsonArray json = doc[setting_key].to<JsonArray>();
            for (const auto &value : setting_vector)
            {
                json.add(value);
            }
        }
        for (const auto &[setting_key, setting_vector] : intSettings)
        {
            JsonArray json = doc[setting_key].to<JsonArray>();
            for (const auto &value : setting_vector)
            {
                json.add(value);
            }
        }
        for (const auto &[setting_key, setting_vector] : boolSettings)
        {
            JsonArray json = doc[setting_key].to<JsonArray>();
            for (const auto &value : setting_vector)
            {
                json.add(value);
            }
        }

        // TODO: WORK HERE!!!!
        return doc;
    }

    // JsonArray lWhite_json = doc["lWhite"].to<JsonArray>();

    // JsonArray lRgb_json = doc["lRgb"].to<JsonArray>();
    //  for (int i = 0; i < L_RGB; i++)
    //  {
    //      lRgb_json.add(lRgb[i].c_str());
    //  }
    //  JsonArray lWhite_json = doc["lWhite"].to<JsonArray>();
    //  for (int i = 0; i < L_WHITE; i++)
    //  {
    //      lWhite_json.add(lWhite[i]);
    //  }
    //  JsonArray mSpeed_json = doc["mSpeed"].to<JsonArray>();
    //  for (int i = 0; i < M_SPEED; i++)
    //  {
    //      mSpeed_json.add(mSpeed[i]);
    //  }

    // map < String, uint
    // return JsonArray();
};

#endif
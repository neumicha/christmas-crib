#include "CCSettings.h"
#include <ArduinoJson.h>
#include "WString.h"

CCSettings::CCSettings()
{
}

CCSettings::CCSettings(String preferences)
{
    JsonDocument settings;
    deserializeJson(settings, preferences);
    String ints[] = {"lWhite", "mSpeed", "aType", "sVolume"};
    String strings[] = {"lRgb", "sSource"};
    String bools[] = {"sState"};

    for (String idString : ints)
    {
        int i = 0;
        Serial.println("Handling INTS");
        while (i < settings[idString].size())
        {
            Serial.print("Length: ");
            Serial.println(settings[idString].size());
            this->intSettings[idString][i] = settings[idString][i];
            i++;
        }
    }
    for (String idString : strings)
    {
        int i = 0;
        Serial.println("Handling STRINGS");
        while (i < settings[idString].size())
        {
            Serial.print("Length: ");
            Serial.println(settings[idString].size());
            this->stringSettings[idString][i] = settings[idString][i].as<String>();
            i++;
        }
    }
    for (String idString : bools)
    {
        int i = 0;
        Serial.println("Handling BOOLS");
        while (i < settings[idString].size())
        {
            Serial.print("Length: ");
            Serial.println(settings[idString].size());
            this->boolSettings[idString][i] = settings[idString][i].as<bool>();
            i++;
        }
    }
    Serial.println("Prefs after loading (in loading): ");
    serializeJsonPretty(this->toJSON(), Serial);
}

JsonDocument CCSettings::toJSON()
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
    return doc;
}

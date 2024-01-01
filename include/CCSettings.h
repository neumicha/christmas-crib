#ifndef CCSETTINGS_H
#define CCSETTINGS_H

#include <string>
#include "WString.h"

struct CCSettings
{
    String color1RGB = "000000";
    uint8_t color1White = 0;
    String color2RGB = "000000";
    uint8_t color2White = 0;
    String color3RGB = "000000";
    uint8_t color3White = 0;
    uint8_t stepper1Speed = 0;
    String color4RGB = "000000";
    uint8_t color4White = 0;
    String sound1Source = "";
    uint8_t sound1Volume = 0;
    bool sound1Status = false;

    String toString()
    {
        return "Colors: [" + color1RGB + ", " + color2RGB + ", " + color3RGB + ", " + color4RGB + "]; " +
               "Whites: [" + color1White + ", " + color2White + ", " + color3White + ", " + color4White + "]; " +
               "Steppers: [" + stepper1Speed + "]; " +
               "SoundSources: [" + sound1Source + "]; " +
               "SoundVolumes: [" + sound1Volume + "]; " +
               "SoundStatus: [" + sound1Status + "]; ";
    }
};

#endif
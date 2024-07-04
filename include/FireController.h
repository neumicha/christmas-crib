#include <Adafruit_NeoPixel.h>

#ifndef FIRECONTROLLER_H
#define FIRECONTROLLER_H

class FireController
{
private:
    Adafruit_NeoPixel *neoPixel;
    int startLed;
    int stopLed;
    long animationFireLastTime;
    int animationInterval;

public:
    FireController(Adafruit_NeoPixel *neoPixel, int startLed, int stopLed);
    void animate(int animation);
};

#endif
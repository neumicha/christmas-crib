#include "FireController.h"
#include <Adafruit_NeoPixel.h>

FireController::FireController(Adafruit_NeoPixel *neoPixel, int startLed, int stopLed)
{
    this->neoPixel = neoPixel;
    this->startLed = startLed;
    this->stopLed = stopLed;
    this->animationFireLastTime = 0;
    this->animationInterval = 0;
};

void FireController::animate(int animation)
{
    // Fire animation
    if (millis() - this->animationFireLastTime >= this->animationInterval)
    {
        switch (animation)
        {
        case 1:
            for (int i = this->startLed; i <= this->stopLed; i++)
            {
                this->neoPixel->setPixelColor(i, this->neoPixel->Color(250, 50, 0, 0));
            }
            this->neoPixel->show();
            break;
        case 2:
            this->animationInterval = random(50, 100);
            for (int i = this->startLed; i <= this->stopLed; i++)
            {
                this->neoPixel->setPixelColor(i, this->neoPixel->Color(random(200, 255), random(30, 70), 0, random(0, 20)));
                if (random(0, 2)) // Random boolean
                {
                    this->neoPixel->setPixelColor(i, this->neoPixel->Color(0, 0, 0, 0));
                }
            }
            this->neoPixel->show();
            break;
        case 3:
            for (int i = this->startLed; i <= this->stopLed; i++)
            {
                this->neoPixel->setPixelColor(i, this->neoPixel->Color(250, 50, 0, 0));
                if (!random(0, 10)) // Random boolean
                {
                    this->neoPixel->setPixelColor(i, this->neoPixel->Color(0, 0, 0, 0));
                }
            }
            this->neoPixel->show();
            break;
        case 4:
            this->animationInterval = random(100, 150);
            for (int i = this->startLed; i <= this->stopLed; i++)
            {
                this->neoPixel->setPixelColor(i, this->neoPixel->Color(random(200, 255), random(30, 70), 0, random(0, 20)));
                if (random(0, 2)) // Random boolean
                {
                    this->neoPixel->setPixelColor(i, this->neoPixel->Color(random(50, 60), random(5, 10), 0, 0));
                }
            }
            this->neoPixel->show();
            break;
        case 5:
            this->animationInterval = random(0, 80);
            for (int i = this->startLed; i <= this->stopLed; i++)
            {
                this->neoPixel->setPixelColor(i, this->neoPixel->Color(random(240, 255), random(40, 60), 0, 0));
                if (!random(0, 10)) // Random boolean
                {
                    this->neoPixel->setPixelColor(i, this->neoPixel->Color(0, 0, 0, 0));
                }
            }
            this->neoPixel->show();
            break;
        default: // Also for 0
            for (int i = this->startLed; i <= this->stopLed; i++)
            {
                this->neoPixel->setPixelColor(i, this->neoPixel->Color(random(200, 255), random(30, 70), 0, random(0, 20)));
            }
            this->neoPixel->show();
            break;
        }
    }
}
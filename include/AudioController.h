#include "Audio.h"

#ifndef AUDIOCONTROLLER_H
#define AUDIOCONTROLLER_H

class AudioController
{
private:
    Audio audio;

public:
    void setup(int i2sBCLK, int i2sLRC, int i2sDOUT, int volume);
    void setVolume(int volume);
    bool connecttoFS(fs::SPIFFSFS spiffs, String file);
    bool connecttohost(String host);
    void loop();
};

#endif
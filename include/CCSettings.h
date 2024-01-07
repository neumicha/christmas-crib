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

struct CCSettings
{
    // Lights
    String lRgb[L_RGB] = {"000000", "000000", "000000", "000000"};
    uint8_t lWhite[L_WHITE] = {0, 0, 0, 0};
    // Motors
    uint8_t mSpeed[M_SPEED] = {0};
    // Animations
    uint8_t aType[A_TYPE] = {0, 0};
    uint8_t aParam[A_PARAM] = {0, 0};
    // Sounds
    String sSource[S_SOURCE] = {""};
    uint8_t sVolume[S_VOLUME] = {3};
    bool sState[S_STATE] = {false};

    String toString()
    {
        ard::ostringstream oss;
        oss << "Lights (rgb): ";
        for (int i = 0; i < L_RGB; i++)
        {
            oss << lRgb[i].c_str() << " | ";
        }
        oss << '\n'
            << "Lights (white): ";
        for (int i = 0; i < L_WHITE; i++)
        {
            oss << std::to_string(lWhite[i]) << " | ";
        }
        oss << '\n'
            << "Motors (speed): ";
        for (int i = 0; i < M_SPEED; i++)
        {
            oss << std::to_string(mSpeed[i]) << " | ";
        }
        oss << '\n'
            << "Animations (type): ";
        for (int i = 0; i < A_TYPE; i++)
        {
            oss << std::to_string(aType[i]) << " | ";
        }
        oss << '\n'
            << "Animations (param): ";
        for (int i = 0; i < A_PARAM; i++)
        {
            oss << std::to_string(aParam[i]) << " | ";
        }
        oss << '\n'
            << "Sounds (source): ";
        for (int i = 0; i < S_SOURCE; i++)
        {
            oss << sSource[i].c_str() << " | ";
        }
        oss << '\n'
            << "Sounds (volume): ";
        for (int i = 0; i < S_VOLUME; i++)
        {
            oss << std::to_string(sVolume[i]) << " | ";
        }
        oss << '\n'
            << "Sounds (state): ";
        for (int i = 0; i < S_STATE; i++)
        {
            oss << std::to_string(sState[i]) << " | ";
        }
        return String(oss.str().c_str());
    }
};

#endif
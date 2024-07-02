#include "AudioController.h"
#include <string>

void AudioController::setup(int i2sBCLK, int i2sLRC, int i2sDOUT, int volume)
{
    audio.setPinout(i2sBCLK, i2sLRC, i2sDOUT);
    this->setVolume(volume);
}
void AudioController::setVolume(int volume)
{
    audio.setVolume(volume);
}
bool AudioController::connecttoFS(fs::SPIFFSFS spiffs, String file)
{
    return audio.connecttoFS(spiffs, file.c_str());
}
bool AudioController::connecttohost(String host)
{
    return audio.connecttohost(host.c_str());
}
void AudioController::loop()
{
    audio.loop();
}
// Audio stuff (optional)
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
}
void audio_id3data(const char *info)
{ // id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ // end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreaminfo(const char *info)
{
    Serial.print("streaminfo  ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ // duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ // homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ // stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}
void audio_eof_speech(const char *info)
{
    Serial.print("eof_speech  ");
    Serial.println(info);
};
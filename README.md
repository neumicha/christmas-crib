# christmas-crib
ESP32 Christmas Crib with support for:
- Neopixel/WS2812/SK6812 LEDs
- Audio (e.g. mp3) playback

## TODOs
- Connect stepper
- Enable WebSerial: https://randomnerdtutorials.com/esp32-webserial-library/
- Fix instability of OTA programming
- Finalize everything (make it production ready)

## How-To

### Access the web interface
The web interface can be accessed via ip-address or via [http://christmas-crib/](http://christmas-crib/)

### Use OTA programming
Add the following lines to your `dev` in `platformio.ini`:
- `upload_port = christmas-crib`
- `upload_protocol = espota`

## Known issues
- We are using fixed version of ASyncTCP (`esphome/AsyncTCP-esphome@2.0.1`) due to dependency issues with the webserver (see [here](https://github.com/esphome/issues/issues/5258))

## Troubleshooting
### ESP32 not recognized as device/in PlatformIO
- Check cable: Are you using a proper USB cable?
- Download [CH340 driver](https://www.wch.cn/download/CH341SER_ZIP.html) (depeding on your ESP32 version) and reconnect device

### Webserver shows error 405
- You might have forgotten to build and upload the filesystem image under `PlatformIO > Platform`

### OTA not working
- Windows users: change WiFi network type to "private" (instead of public)
- Disable firewall
- Use good power supply (USB of laptop might not be sufficient)

## Ressources
- [Websocket + Sliders](https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/)
- [Color Picker](https://randomnerdtutorials.com/esp32-esp8266-rgb-led-strip-web-server/)
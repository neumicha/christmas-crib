# christmas-crib
ESP32 Christmas Crib with support for:
- Neopixel/WS2812/SK6812 LEDs
- Audio (e.g. mp3) playback
- Stepper motors

Code is not optimized for performance but instead for easy changing etc.

## TODOs
- Save and restore settings
- Create Favicon
- Flickering lights
- Enable WebSerial: https://randomnerdtutorials.com/esp32-webserial-library/
- Fix instability of OTA programming
- Finalize everything (make it production ready)
- Optimize performance by replacing String class by standard C functions etc. 
- Make preset selectable (implement presets)

## How-To

### Connect everything
|ESP32 Pin|Connected to|
|--------|--------|
| D16 | Neopixel LED - Data |
| D18 | Max98357A - LRC |
| D19 | Max98357A - BCLK |
| D21 | Max98357A - DIN |
| D25 | ULN2003 - IN1 |
| D26 | ULN2003 - IN2 |
| D27 | ULN2003 - IN3 |
| D14 | ULN2003 - IN4 |

Furthermore, connect SD of Max98357A to 5V.

### Access the web interface
The web interface can be accessed via ip-address or via [http://christmas-crib/](http://christmas-crib/)

### Use OTA programming
Add the following lines to your `dev` in `platformio.ini`:
- `upload_port = christmas-crib`
- `upload_protocol = espota`

## Nice2Know
### Naming convention of actuators etc.
- l: Light (rgb, white)
- m: Motor (speed)
- a: Animation (type, param)
- s: Sound (source, state, volume)
- p: Preset (save, load, reset)

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
- ESP32 Pinout: [1](https://www.electronicshub.org/esp32-pinout/), [2](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/), [3](https://circuits4you.com/2018/12/31/esp32-devkit-esp32-wroom-gpio-pinout/)
- [Websocket + Sliders](https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/)
- [Color Picker](https://randomnerdtutorials.com/esp32-esp8266-rgb-led-strip-web-server/)
- [Fire simulation](https://www.az-delivery.de/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/mehrere-feuer-programme-fuer-unsere-stimmungslaterne)
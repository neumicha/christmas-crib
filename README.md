# christmas-crib
ESP32 Christmas Crib

## TODOs
- Connect Neopixel LEDs
- Build Web UI
- Connect stepper
- Connect sound
- Enable WebSerial: https://randomnerdtutorials.com/esp32-webserial-library/
- Fix instability of OTA programming
- Finalize everything (make it production ready)

## How-To
### Use OTA programming
Add the following lines to your `dev` in `platformio.ini`:
- `upload_port = christmas-crib`
- `upload_protocol = espota`

## Troubleshooting
### ESP32 not recognized as device/in PlatformIO
- Check cable: Are you using a proper USB cable?
- Download [CH340 driver](https://www.wch.cn/download/CH341SER_ZIP.html) (depeding on your ESP32 version) and reconnect device

### OTA not working
- Windows users: change WiFi network type to "private" (instead of public)
- Disable firewall
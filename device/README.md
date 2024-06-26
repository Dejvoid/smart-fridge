# Instructions for board setup
## Before Build
Before building you need to set WiFi SSID and Password and MQTT URI in `main.cpp`


## Build environment
- [esp-idf](https://github.com/espressif/esp-idf) (tested on version 5.1.2) 
    - I personally used VSCode with esp-idf extension

## Wiring
![Wiring](../connection.drawio.png)

## Build process (using VSCode)
1. Clone repository
2. `cd smart-fridge/device/`
3. `code .`
4. Fill wifi SSID and password, MQTT server URI. Edit pin numbers if needed.
5. Copy generated certificates `ca.crt`, `device.key`, `device.crt` to `main` folder
6. Open vscode quick-launch (`Ctrl+Shift+P`)
7. `ESP-IDF: Build your project` (assuming ESP-IDF extension to be configured)
    - Or `ESP-IDF: Build, Flash and Start Monitor on your Device` to directly flash and monitor.

## Controls
- Board is currently responsive to 3 console commands:
    - `add product` - starts scanning for the codes. after successful scan, the device sends the code to the server as MQTT publish to the topic `products/add`
    - `rm product` - starts scanning for the codes. after successful scan, the device sends the code to the server as MQTT publish to the topic `products/rm`
    - `stop scan` - manually stops scanning. No code is read

- Board is permanently connected to given server. If connection fails it tries to reconnect every second. If any data is received they are interpreted as notifications on the display.

- Any message received from the server will be displayed on the bottom of the display.

### Types of notifications:
- No type - white color
- Low priority (1) - green color
- Medium priority (2) - yellow color
- High priority (3) - red color

### Display layout
- Camera preview when scanning
- Temperature and humidity
- Notifications

### Server connection
The board has continuously open connection to the web server. If the connection fails the board tries to reconnect automatically.

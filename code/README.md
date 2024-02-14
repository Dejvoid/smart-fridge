# Instructions for board setup

## Build environment
- [esp-idf](https://github.com/espressif/esp-idf) (tested on version 5.1.2) 
    - I personally used VSCode with esp-idf extension

## Wiring
![Wiring](../connection.drawio.png)

## Build process
1. Clone repository
2. `cd smart-fridge/code/`
3. `code .`
4. Fill wifi SSID and password
5. Open vscode quick-launch (`Ctrl+Shift+P`)
6. `ESP-IDF: Build your project` (assuming ESP-IDF extension to be configured)
    - Or `ESP-IDF: Build, Flash and Start Monitor on your Device` to directly flash and monitor.

## Controls
- Board is currently responsive to 3 console commands:
    - `send` - sends example temperature and humidity data to the server
    - `start scan` - starts QR code scanning. Shows camera view on the display. After succhessful scan of the code, the scan automatically stops and sends scanned data to the server
    - `stop scan` - manually stops scanning. No QR code is read

- Board is permanently connected to given server. If connection fails it tries to reconnect every second. If any data is received they are interpreted as notifications on the display.

### Types of notifications:
- No type - white color
- Low priority()
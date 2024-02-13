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

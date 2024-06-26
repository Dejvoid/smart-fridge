# Instructions for board setup
## Before Build
Before building you need to set WiFi SSID and Password and MQTT URI in `main.cpp`

It is also necessary to create your certificates. 

### Certification Authority certificate
1) Generate key and certificate: `openssl req -x509 -newkey rsa:4096 -keyout ca.key -out ca.crt -nodes -sha256 -days 60`
    - Fill country, company, and [common name]
2) (Optional) Add as trusted CA

### Server certificate
0) Write information for the certificate `openssl req -new -nodes -out server.csr -newkey rsa:4096 -keyout server.key -config san.cnf`

1) Generate key and certificate: 
    - Fill country, company, and common name
2) Sign the certificate: `openssl x509 -req -in server.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out server.crt -days 730 -sha256 -extensions req_ext -extfile san.cnf` !!! sign with the cnf file, mbed tls on esp buggy with tls v1.3
3) Convert certificate: `openssl pkcs12 -export -out server.pfx -inkey server.key -in server.crt`

### Device certificate
1) Generate key and certificate request: `openssl req -new -nodes -out device.csr -newkey rsa:2048 -keyout device.key`
2) Sign the certificate: `openssl x509 -req -in device.csr -CA ca.crt -CAkey ca.key -CAcreateserial -out device.crt -days 730 -sha256`


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

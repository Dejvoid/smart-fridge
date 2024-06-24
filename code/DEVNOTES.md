# Notes during development

## AI-Thinker's ESP32-CAM

- Reset button on the "bottom" of the board so it is inaccessible when in breadboard

- No reset pin

- Out of 16 pins 6 pins are used for either GND or 5V or 3.3V.

## Display

- Documentation of esp-idf on SPI is sometimes a bit confusing
    - `MOSI` pin is the same thing as `data0_io_num` in `spi_bus_config_t`

- Datasheet for ILI9488 states that 5-6-5 color coding is available for 4-wire SPI however it is not really supported

- Minimizing pins used by the display

- Compromise on SPI "ballance" buffer size

## Camera

- Buffer orientation -> printing on display into square ends up being 2 triangles -> was caused by error in setting width of drawable area (+-1 error)

- Choose the right resolution to fit in memory yet have good quality

## Thermometer

- Decision between communicating one-wire over U(S)ART or bare timer

    - difficulty of implementation

    - 1 vs 2 pins needed

## Server connection

- Choosing how to treat connection (tasks or different approach)
- Giving up on HTTP using pure sockets

## Debugging

- Can't connect J-Tag because pins are used for display (if I want display I can't use J-Tag and vice versa)

- Using Logic Analyzer

    - Turned out to work quite well for debugging of SPI, One-Wire

- https://esp32.com/viewtopic.php?t=28028

## MQTT vs socket
- socket very simple and quite easy to implement
- mqtt troublesome but working standard
    - issues: disconnecting (not finishing 3-way handshake) - maybe issue with security?



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

openssl genrsa -out client.key
openssl req -out client.csr -key client.key -new

Country Name (2 letter code) [XX]:CZ
Organization Name (eg, company) [Default Company Ltd]:smart-fridge
Common Name (eg, your name or your server's hostname) []:device


openssl req -x509 -newkey rsa:4096 -keyout server.key -out server.crt -nodes -sha256 -days 60

Country Name (2 letter code) [XX]:cz
Organization Name (eg, company) [Default Company Ltd]:smart-fridge
Common Name (eg, your name or your server's hostname) []:server
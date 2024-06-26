# smart-fridge

## Description
Smart fridge project running on ESP32-CAM and Linux server (Raspberry PI).

### Parts
- ESP32-CAM board
- ILI9488 LCD
- (AHT10 thermometer)
- Raspberry PI or other Linux machine

## Deployment
It is necessary to create your certificates before deployment. 

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

After certificates are created follow instructions for device installation: See `code/README.md`

For the server deployment:
1) Build the server (within `server/Server` directory): `dotnet build -c Release -o bin --sc -a arm64 --os linux`
    - architecture parameter (`-a arm64`) and OS (`--os linux`) depends on your architecture and operating system
2) Copy `ca.crt` and `server.pfx` to the `bin` directory
3) run using `./Server`


## Authors and acknowledgment
David Hrivna

## Project status
Being worked on

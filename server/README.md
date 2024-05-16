# Server
The server is custom HTTP server with TCP socket for communication witht the device. Database connection is ensured via Entity Framework to the local SQL server.

## Architecture
The architecture of the server consists of 3 main parts. HTTP Server, Socket Server and DB access.

## Web layout
The server has 3 main parts:
- Products
- Recipes

## Usage
The server listens on all IP addresses and provides HTTP access to web pages via port 80. 
The socket for device is listening on port 2666.

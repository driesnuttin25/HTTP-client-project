# UnoReverse - TCP Server and HTTP Client
UnoReverse is a program consisting of a TCP server and an HTTP client that listens on port 22 for new connections and detects the IP address of the attacker (i.e. bot-net). It logs the login attempt in a log file along with the received data, network statistics, and the geo-location using IP Geolocation API. For the latter, the program uses an HTTP client to communicate with the API and store the data returned in the log. Furthermore, the server will perform a reverse attack by sending back a large amount of data.

## Checklist
- [x] TCP server listens on port 22.
- [x] TCP server accepts multiple connections.
- [x] TCP server detects the IP address of the client.
- [x] TCP server logs the IP address in logs.
- [x] TCP server starts an HTTP client for each new connection.
- [x] HTTP client makes a TCP connection with IP Geolocation API.
- [x] HTTP client sends a correct HTTP GET request.
- [x] HTTP client receives the response from the HTTP server.
- [x] HTTP client extracts the pure full JSON response.
- [x] HTTP client logs the geolocation in logs.
- [x] HTTP client closes the connection cleanly.
- [x] TCP server accepts data from the client and logs it in logs.
- [ ] TCP server sends as much data as possible to the open connection.
- [x] TCP server keeps track of how many data has been successfully delivered and logs it when closing the connection.
- [x] TCP server closes the connection after the client closes the connection.
- [ ] TCP server can handle multiple connections simultaneously.
- [ ] The entire program works without crashing and uses computer resources efficiently (i.e. memory and sockets).
- [x] Code is professionally stored on GitHub (i.e. multiple useful commits and explanations).
- [ ] The program works and is available on the internet (e.g. at home using NAT or works on a public server).
 ## Additional nice-to-have features:
Documentation using flowchart (see miro).
Use of pthread.
IPs are stored in a lookup table (e.g. linked-list) to log repeated attacks.
## How to use
To use UnoReverse, follow these steps:
- Clone this repository to your local machine.
- Compile the program using your preferred compiler.
- Run the program using the command line, specifying the port number to listen on (defaults to port 22).

## Credits
This project was created by Dries Nuttin.

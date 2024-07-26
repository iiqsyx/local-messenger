# Server 
This is a simple server for the messenger. For work with sockets i've picked up <winsock2.h> library.

# Technology
To start the program you must have CMake with minimum required version 3.24.. To build the program you should follow this steps in console:
- D:\...\local-messenger\Server(go to the project folder)
- mkdir build
- cd build
- cmake ../
- cmake --build

# Server in general
Program initialize [WSA](https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup). Server binds socket with IPv4 and port, server start [listening](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-listen) on socket and [accepting](https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-accept) clients.

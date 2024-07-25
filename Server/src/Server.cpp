#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>

#pragma warning(disable: 4996)

SOCKET Connections[3];
int Counter = 0;

void ClientHandler(int index) {
    int msgSize;
    while (true) {
        recv(Connections[index], (char*)&msgSize, sizeof(int), NULL);
        char* msg = new char[msgSize+1];
        msg[msgSize] = '\0';

        if (recv(Connections[index], msg, sizeof(msg), NULL) == SOCKET_ERROR) {
            closesocket(Connections[index]);
            return;
        }
        for (int i = 0; i < Counter; i++) {
            if (index == i) {
                continue;
            }
            send(Connections[i], (char*)&msgSize, sizeof(int), NULL);
            send(Connections[i], msg, msgSize, NULL);
        }
        
        delete[]msg;
    }
}

class Server
{
public:
    Server() : serverSocket(INVALID_SOCKET) {
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    };
    ~Server() {
        closesocket(serverSocket);
    };
    
    void bindSocket(const char* ip, u_short port) {
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip);
        addr.sin_port = htons(port);

        if (bind(serverSocket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            throw("ERROR\tbind faild : " + WSAGetLastError());
        }
    }

    void listenSocket() {
        if (listen(serverSocket, 2) == SOCKET_ERROR) {
            throw("ERROR\tlisten on socket faild : " + WSAGetLastError());
        }
    }

    void acceptingClients() {
        SOCKET acceptSocket;
        for (int i = 0; i < 3; i++) {
            acceptSocket = accept(serverSocket, NULL, NULL);

            if (acceptSocket == INVALID_SOCKET) {
                throw("ERROR\taccept faild : " + WSAGetLastError());
            }
            else {
                std::cout << "NEW CLIENT CONNECTED" << std::endl;

                Connections[i] = acceptSocket;
                Counter++;
                CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), 0, 0);
            }
        }
    }
    
private:
    SOCKET serverSocket;
    SOCKADDR_IN addr;
    char buffer[256];
};

int main() {
    // STEP ONE: INITIALIZE WSA
    WSADATA wsaData;
    WORD DLLVersion = MAKEWORD(2,2);

    if (WSAStartup(DLLVersion, &wsaData)) {
        std::cerr << "----------------------------------\n"
            << "|ERROR\tthe Winsock dll not found|\n"
            << "----------------------------------\n";
        exit(1);
    }
    try {
        Server server;

        server.bindSocket("127.0.0.1", 1111);
        server.listenSocket();
        server.acceptingClients();
    }
    catch (const char* exceptionMsg) {
        std::cout << exceptionMsg << std::endl;
    }
    

    // CLEAN RESOURSES
    WSACleanup();
    return 0;
}


#include <iostream>
#include <WinSock2.h>
#include <afxres.h>

using namespace std;

int main() {
    cout << "Client start." << endl;
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData{};

    // Load winsocket dll
    cout << "Loading..." << endl;
    if (WSAStartup(sockVersion, &wsaData) == SOCKET_ERROR) {
        cout << "Error occurred in initialization." << endl;
        return -1;
    } else {
        // Check the lowest and highest byte of the version in HEX
        if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
            cout << "Could not find Winsock 2.2 dll." << endl;
            WSACleanup();
            return -1;
        }
    }
    cout << "Loading...OK" << endl;

    // Create socket based on TCP
    cout << "Creating..." << endl;
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET) {
        cout << "Error occurred in creating socket." << endl;
        WSACleanup();
        return -1;
    }
    cout << "Creating...OK" << endl;

    // Prepare for the connection
    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5555);
    servAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(s, (sockaddr*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        cout << "Error occurred in Connecting." << endl;
        WSACleanup();
        return -1;
    }

    char buff[156];
    int nRecv = recv(s, buff, 156, 0);
    if (nRecv > 0) {
        buff[nRecv] = '\0';
        cout << "Data recieved: " << buff << endl;
    }

    closesocket(s);
    WSACleanup();
    return 0;

    return 0;
}
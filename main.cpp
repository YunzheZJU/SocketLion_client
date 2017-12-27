#include "main.h"

SOCKET socketServer;
string number;
string address;
string port;

int main() {
    cout << "Client start." << endl;

    bool outerLoop = true;
    char selection;

    cout << "Welcome to SocketLion v1.0!\nEnter a number to select an item below." << endl;
    while (outerLoop) {
        cout << "Menu:\n[1] Connect to the server.\n[2] Exit." << endl;
        scanf("%c", &selection);
        getchar();
        switch (selection) {
            case '1':
                if (Connect()) {
                    cout << "Successfully connected to the server!" << endl;
                    cout << "Your user number is [" << number << "], " << address << ":" << port << endl;
                    bool innerLoop = true;
                    while (innerLoop) {
                        cout << "Menu:\n"
                                "[1] Request for server time.\n"
                                "[2] Request for server name.\n"
                                "[3] Request for online users.\n"
                                "[4] Send a message to user X.\n"
                                "[5] Disconnect."
                             << endl;
                        scanf("%c", &selection);
                        getchar();
                        switch (selection) {
                            case '1':
                                if (!GetTime()) {
                                    cout << "Fail to get server time!" << endl;
                                }
                                break;
                            case '2':
                                if (!GetServer()) {
                                    cout << "Fail to get server name!" << endl;
                                }
                                break;
                            case '3':
                                if (!GetList()) {
                                    cout << "Fail to get list of online users!" << endl;
                                }
                                break;
                            case '4':
                                Request();
                                break;
                            case '5':
                                if (Disconnect()) {
                                    cout << "Successfully disconnected to the server!" << endl;
                                }
                                innerLoop = false;
                                break;
                            default:
                                cout << "Unknown input: '" << selection << "'. Please try again." << endl;
                                break;
                        }
                    }
                }
                break;
            case '2':
                outerLoop = false;
                break;
            default:
                cout << "Unknown input: '" << selection << "'. Please try again." << endl;
                break;
        }
    }
    cout << "Bye." << endl;
    return 0;
}

bool Connect() {
    WORD version = MAKEWORD(2, 2);
    WSADATA data{};

    // Load winsocket dll
    clog << "Loading..." << endl;
    if (WSAStartup(version, &data) == SOCKET_ERROR) {
        cerr << "Error occurred in initialization: " << WSAGetLastError() << "." << endl;
        return false;
    } else {
        // Check the lowest and highest byte of the version in HEX
        if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2) {
            cerr << "Could not find a usable version of Winsock.dll: " << WSAGetLastError() << "." << endl;
            WSACleanup();
            return false;
        }
    }
    clog << "Loading...OK" << endl;

    // Create socket based on TCP
    clog << "Creating..." << endl;
    socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketServer == INVALID_SOCKET) {
        cerr << "Error occurred in creating socket: " << WSAGetLastError() << "." << endl;
        WSACleanup();
        return false;
    }
    clog << "Creating...OK" << endl;

    // Prepare for the connection
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    serverAddress.sin_addr.S_un.S_addr = inet_addr(SERVER_ADDRESS);

    // Connect to the server
    clog << "Connecting..." << endl;
    if (connect(socketServer, (sockaddr *) &serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        cerr << "Error occurred in Connecting: " << WSAGetLastError() << "." << endl;
        WSACleanup();
        return false;
    }
    clog << "Connecting...OK" << endl;

    clog << "ALOHA..." << endl;
    char request[] = "ALOHA\r\n\r\n";
    char response[256];
    string headerNumber = "Number";
    string headerAdress = "Address";
    string headerPort = "Port";
    if (Request(request, response)) {
        number = GetHeader(response, headerNumber);
        address = GetHeader(response, headerAdress);
        port = GetHeader(response, headerPort);
        clog << "ALOHA...OK" << endl;
        return true;
    }
    return false;
}

bool Disconnect() {
    closesocket(socketServer);
    WSACleanup();
    return true;
}

bool GetTime() {
    char request[] = "ALOHA\r\n\r\n";
    char response[256];
    string header = "Time";
    if (Request(request, response)) {
        const string time = GetHeader(response, header);
        cout << time << endl;
        return true;
    }
    return false;
}

bool GetServer() {
    char request[] = "ALOHA\r\n\r\n";
    char response[256];
    string header = "Server";
    if (Request(request, response)) {
        const string server = GetHeader(response, header);
        cout << server << endl;
        return true;
    }
    return false;
}

bool GetList() {
    char request[] = "LIST\r\n\r\n";
    char response[256];
    // TODO
    return Request(request, response);
}

bool Request() {
    char request[] = "SEND\r\n\r\n";
    char response[256];
    // TODO
    return Request(request, response);
}

bool Request(const char request[], char response[]) {
    send(socketServer, request, strlen(request), 0);

    int responseLength = recv(socketServer, response, 256, 0);
    if (responseLength > 0) {
        response[responseLength] = '\0';
        clog << "Response: " << response << endl;
    }

    return true;
}

string GetHeader(const string &response, const string &header) {
    string stringToFind = header + ": ";
    string temp = response.substr(response.find(stringToFind) + stringToFind.length());
    return temp.substr(0, temp.find('\r'));
}

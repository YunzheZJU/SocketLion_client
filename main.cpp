#include "main.h"

SOCKET socketServer;
string number;
string address;
string port;
queue<string> messageQueue;
thread threadSend;
thread threadReceive;
bool stopThread = true;

int main() {
    cout << "Client start." << endl;

    bool loop = true;
    char selection;

    cout << "Welcome to SocketLion v1.0!\nEnter a number to select an item below." << endl;
    while (loop) {
        cout << "Menu:\n[1] Connect to the server.\n[2] Exit." << endl;
        scanf("%c", &selection);
        getchar();

        switch (selection) {
            case '1':
                if (Connect()) {
                    clog << "Thread Send starts." << endl;
                    cout << "Successfully connected to the server!" << endl;
                    threadSend = thread(Send);
                    threadReceive = thread(Receive);
                    stopThread = false;
                }
                break;
            case '2':
                loop = false;
                break;
            default:
                cout << "Unknown input: '" << selection << "'. Please try again." << endl;
                break;
        }

        while (!stopThread) {
            // Check message
            if (!messageQueue.empty()) {
                string message = messageQueue.front();
                messageQueue.pop();
                cout << "MessageQueue: " << endl << message << endl;
            } else {
                Sleep(100);
            }

        }
    }
    if (threadReceive.joinable()) {
        threadReceive.join();
    }
    if (threadSend.joinable()) {
        threadSend.join();
    }
    cout << "Bye." << endl;
    return 0;
}

void Send() {
    clog << "Thread Send starts." << endl;
    char selection;
    while (!stopThread) {
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
                SendMsg();
                break;
            case '5':
                if (Disconnect()) {
                    cout << "Successfully disconnected to the server!" << endl;
                }
                stopThread = true;
                break;
            default:
                cout << "Unknown input: '" << selection << "'. Please try again." << endl;
                break;
        }
    }
    clog << "Thread Send exits." << endl;
}

void Receive() {
    clog << "Thread Receive starts." << endl;
    char response[256];
    int retryCount = 0;
    while (!stopThread) {
        int responseLength = recv(socketServer, response, 256, 0);
        if (responseLength > 0) {
            response[responseLength] = '\0';
            clog << "Response: " << endl << response << endl;
            string message = AnalyzeResponse(response);
            messageQueue.push(message);
            retryCount = 0;
        } else if (responseLength == 0 || retryCount == 10) {
            clog << "The server has closed the connection!" << endl;
            stopThread = true;
        } else {
            retryCount++;
            clog << "Error occurred when receiving: " << WSAGetLastError() << endl;
            Sleep(500);
        }
    }
    clog << "Thread Receive exits." << endl;
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
    Aloha();
    return true;
}

bool Disconnect() {
    closesocket(socketServer);
    WSACleanup();
    return true;
}

bool Aloha() {
    const char request[] = "ALOHA\r\n\r\n";
    return Request(request);
}

bool GetTime() {
    char request[] = "TIME\r\n\r\n";
    return Request(request);
}

bool GetServer() {
    char request[] = "SERV\r\n\r\n";
    return Request(request);
}

bool GetList() {
    char request[] = "LIST\r\n\r\n";
    return Request(request);
}

bool SendMsg() {
    char request[] = "SEND\r\nToNumber: 0\r\nToAddress: 127.0.0.1\r\n\r\nHello";
    return Request(request);
}

bool Request(const char request[]) {
    clog << "Sending..." << endl;
    send(socketServer, request, static_cast<int>(strlen(request)), 0);
    clog << "Sending...OK" << endl;
    return true;
}

string AnalyzeResponse(const char response[]) {
    string stringResponse = response;
    string stringFront = stringResponse.substr(0, stringResponse.find('\r'));
    // This is a response
    string method = GetValue(stringResponse, "Method");
    if (stringFront == "302") {
        string keywordFromNumber = "FromNumber";
        string keywordFromAddress = "FromeAddress";
        string fromNumber = GetValue(stringResponse, keywordFromNumber);
        string fromAddress = GetValue(stringResponse, keywordFromAddress);
        string message = GetContent(stringResponse);
        string newResponse;
        newResponse.append("REPLY\r\n");
        newResponse.append("ToNumber: " + fromNumber + "\r\n");
        newResponse.append("ToAddress: " + fromAddress + "\r\n");
        newResponse.append("\r\n");
        newResponse.append(message);
        Request(newResponse.data());
        return "A new message from user [" + fromNumber + "]: \n" + message;
    } else if (stringFront == "200") {
        if (method == "ALOHA") {
            clog << "ALOHA...OK" << endl;
            number = GetValue(stringResponse, "Number");
            address = GetValue(stringResponse, "Address");
            port = GetValue(stringResponse, "Port");
            return "Your user number is [" + number + "], " + address + ":" + port;
        } else if (method == "TIME" || method == "SERV" || method == "LIST") {
            return GetContent(stringResponse, method);
        } else if (method == "SEND") {
            string content = GetContent(stringResponse, method);
            if (content == "200") {
                return "Message is delivered successfully.";
            } else if (content == "500") {
                return "Failed to receive the reply of target user.";
            } else if (content == "502") {
                return "Failed to send message to target user. Maybe he/she is offline just now.";
            }
        } else if (method == "REPLY") {
            // TODO: Receive the check message.
        }
    } else if (stringFront == "404") {
        if (method == "SEND") {
            return "That user is offline or does not appear. Please try again!";
        }
    } else if (stringFront == "400") {
        return "The server receives an unknown method: " + method;
    }
}

string GetValue(const string &response, const string &keyword) {
    string stringToFind = keyword + ": ";
    string temp = response.substr(response.find(stringToFind) + stringToFind.length());
    return temp.substr(0, temp.find('\n'));
}

string GetContent(const string &response, const string &method) {
    return response.substr(response.find(method) + method.length() + 1);
}

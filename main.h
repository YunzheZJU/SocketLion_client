//
// Created by Yunzhe on 2017/12/25.
//

#ifndef SOCKETLION_CLIENT_MAIN_H
#define SOCKETLION_CLIENT_MAIN_H

#include <iostream>
#include <WinSock2.h>

using namespace std;

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 1551

bool Connect();

bool Disconnect();

bool GetTime();

bool GetServer();

bool GetList();

bool Request();

bool Request(const char request[], char response[]);

string GetHeader(const string &response, const string &header);

#endif //SOCKETLION_CLIENT_MAIN_H

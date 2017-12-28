//
// Created by Yunzhe on 2017/12/25.
//

#ifndef SOCKETLION_CLIENT_MAIN_H
#define SOCKETLION_CLIENT_MAIN_H

#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <queue>

using namespace std;

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 4084

void Send();

bool Connect();

void Receive();

bool Disconnect();

bool Aloha();

bool GetTime();

bool GetServer();

bool GetList();

bool SendMsg();

bool Request(const char request[]);

string AnalyzeResponse(const char response[]);

string GetValue(const string &response, const string &keyword, const char separater = '\r');

string GetContent(const string &response, const string &method = "\r\n\r");

#endif //SOCKETLION_CLIENT_MAIN_H

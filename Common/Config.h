#pragma once
#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include "WinsockInit.h"
#include "ClientSocket.h"

using namespace std;

constexpr unsigned short SERVER_PORT = 9999;
constexpr const char* SERVER_IP = "127.0.0.1";
constexpr const int BUF_SIZE = 1024;

typedef struct tagClientInfo {
	int id;
	shared_ptr<ClientSocket> socket;
	std::string nickname; 
}CLIENT_INFO;
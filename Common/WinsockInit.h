#pragma once
#include <iostream>
#include <memory>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

class WinsockInit {
public:
    ~WinsockInit() { WSACleanup(); }

    WinsockInit(const WinsockInit&) = delete;
    WinsockInit& operator=(const WinsockInit&) = delete;

    static std::unique_ptr<WinsockInit> Create() {
        WSAData wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0)
        {
            std::cout << "WSAStartup failed, error code : " << result << "\n";
            return nullptr;
        }

        return std::unique_ptr<WinsockInit>(new WinsockInit());
    }

private:
    WinsockInit() = default;
};
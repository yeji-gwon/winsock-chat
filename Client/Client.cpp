#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <memory>
#include <string>
#include "WinsockInit.h"
#include "ConnectSocket.h"
#include "Config.h" 
using namespace std;

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Winsock 초기화
    unique_ptr<WinsockInit> winsockInit = WinsockInit::Create();
    if (!winsockInit)
        return 1;

    unique_ptr<ConnectSocket> connectSocket = ConnectSocket::Create();
    if (!connectSocket)
        return 1;

    if (!connectSocket->Connect("127.0.0.1", SERVER_PORT))
        return 1;

    cout << "서버에 접속\n";
    cout << "아무 키나 누르면 종료합니다...\n";
    cin.get();  // 키 입력 대기

    return 0;
}
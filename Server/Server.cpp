#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <memory>
#include "WinsockInit.h"
#include "ListenSocket.h"
#include "Config.h"
using namespace std;
 
int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Winsock 초기화
    unique_ptr<WinsockInit> winsockInit = WinsockInit::Create();
    if (!winsockInit)
        return 1;
    
    // 소켓 생성
    unique_ptr<ListenSocket> listenSocket = ListenSocket::Create();
    if (!listenSocket)
        return 1;

    // 소켓 바인딩. IP 주소, 포트 번호 할당
    if (!listenSocket->Bind(SERVER_PORT))
        return 1;
    
    // 소켓을 접속 대기 상태로 전환.
    if (!listenSocket->Listen())
        return 1;

    return 0;
}
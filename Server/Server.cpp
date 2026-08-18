#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>
#include <iostream>
#include <memory>
#include <thread>
#include "WinsockInit.h"
#include "ListenSocket.h"
#include "Config.h"

using namespace std;

bool running = true;

void AcceptLoop(const unique_ptr<ListenSocket>& listenSocket);

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Winsock 초기화
    unique_ptr<WinsockInit> winsockInit = WinsockInit::Create();
    if (!winsockInit)
        return 1;
    
    // Listen 소켓 생성
    unique_ptr<ListenSocket> listenSocket = ListenSocket::Create();
    if (!listenSocket)
        return 1;

    // Listen 소켓 바인딩. IP 주소, 포트 번호 할당
    if (!listenSocket->Bind(SERVER_PORT))
        return 1;
    
    // Listen 소켓을 접속 대기 상태로 전환.
    if (!listenSocket->Listen())
        return 1;

    thread acceptThread(AcceptLoop, ref(listenSocket));

    acceptThread.join();

    return 0;
}

void AcceptLoop(const unique_ptr<ListenSocket>& listenSocket)
{
    while (running)
    {
        auto client = listenSocket->Accept();

        if (!client)
        {
            cout << "[서버] accept 실패 : \n";
            break;
        } 

        cout << "[서버] 클라이언트 접속\n";

        while (true)
        {
            // 클라이언트로부터 메시지 수신 후 그대로 반환 (echo)
            char buffer[512] = {};
            int received = client->Recv(buffer, sizeof(buffer) - 1);

            if (received <= 0)
                break;

            buffer[received] = '\0';
            cout << "받은 메시지 : " << buffer << "\n";
            client->Send(buffer, received);
        }
    }
}
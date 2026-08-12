#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include "WinsockInit.h"
#include "ConnectSocket.h"
#include "ClientSocket.h"
#include "Config.h" 
using namespace std;

atomic<bool> running = true;

void RecvLoop(ClientSocket* client);

int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Winsock 초기화
    unique_ptr<WinsockInit> winsockInit = WinsockInit::Create();
    if (!winsockInit)
        return 1;

    // 소켓 생성
    unique_ptr<ConnectSocket> connectSocket = ConnectSocket::Create();
    if (!connectSocket)
        return 1;

    // 서버에 연결 요청
    if (!connectSocket->Connect(SERVER_IP, SERVER_PORT))
        return 1;
    cout << "서버에 접속\n";

    // 연결된 소켓의 소유권을 ClientSocket으로 이전 (이후 recv/send 담당)
    auto client = ClientSocket::Create(connectSocket->Release());

    thread recvThread(RecvLoop, client.get());

    while (running)
    {
        // 메시지 입력
        string msg;
        cout << "보낼 메시지 입력 (/quit 종료) : ";
        getline(cin, msg);

        if (!running)
            break;

        if (msg == "/quit")
        {
            running = false;
            break;
        } 

        client->Send(msg.c_str(), static_cast<int>(msg.length()));
    }

    client->Shutdown();
    recvThread.join();

    return 0;
}

void RecvLoop(ClientSocket* client)
{
    while (running)
    {
        char buffer[512] = {};
        int received = client->Recv(buffer, sizeof(buffer) - 1);
        if (received <= 0)
        {
            cout << "\n서버와 연결이 종료되었습니다.\n";
            running = false;
            break;
        } 

        buffer[received] = '\0';
        cout << "서버로부터 받은 메시지 : " << buffer << "\n";
    }
}
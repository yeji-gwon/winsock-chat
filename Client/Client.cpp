#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <memory>
#include <string>
#include "WinsockInit.h"
#include "ConnectSocket.h"
#include "ClientSocket.h"
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
    unique_ptr<ConnectSocket> connectSocket = ConnectSocket::Create();
    if (!connectSocket)
        return 1;

    // 서버에 연결 요청
    if (!connectSocket->Connect(SERVER_IP, SERVER_PORT))
        return 1;
    cout << "서버에 접속\n";

    // 연결된 소켓의 소유권을 ClientSocket으로 이전 (이후 recv/send 담당)
    auto client = ClientSocket::Create(connectSocket->Release());

    // 메시지 입력받아 서버로 전송
    string msg;
    cout << "보낼 메시지 입력 : ";
    getline(cin, msg);
    client->Send(msg.c_str(), static_cast<int>(msg.length()));

    // 서버로부터 응답 수신
    char buffer[512] = {};
    int received = client->Recv(buffer, sizeof(buffer) - 1);
    if (received > 0)
    {
        buffer[received] = '\0';
        cout << "서버로부터 받은 메시지 : " << buffer << "\n";
    }
     
    cout << "아무 키나 누르면 종료합니다...\n";
    cin.get();  // 키 입력 대기

    return 0;
}
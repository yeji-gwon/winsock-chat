#include "Config.h" 
#include "ConnectSocket.h"

atomic<bool> g_connected = true;

void RecvLoop(const unique_ptr<ClientSocket>& clientSocket);

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

    // 연결된 소켓의 소유권을 ClientSocket으로 이전 (이후 recv/send 담당)
    auto client = ClientSocket::Create(connectSocket->Release());

    string nickname;
    cout << "사용할 닉네임을 입력하세요 : ";
    getline(cin, nickname);
    if (nickname.empty())
        nickname = "익명";
    client->Send(nickname.c_str(), static_cast<int>(nickname.size()));

    cout << "서버에 연결되었습니다. 메시지를 입력하세요 (종료: /quit)" << endl;

    thread recvThread(RecvLoop, ref(client));

    string line;
    while (g_connected && getline(cin, line))
    {
        if (line == "/quit")
            break;
        if (line.empty())
            continue;

        if (SOCKET_ERROR == client->Send(line.c_str(), static_cast<int>(line.length())))
        {
            cout << "[클라이언트] 메시지 전송 실패" << endl;
            break;
        } 
    }

    g_connected = false;

    client->Shutdown();
    client->Close();

    recvThread.join();

    return 0;
}

void RecvLoop(const unique_ptr<ClientSocket>& clientSocket)
{
    while (g_connected)
    {
        char buffer[BUF_SIZE] = {};
        int received = clientSocket->Recv(buffer, BUF_SIZE - 1);
        if (received <= 0)
        {
            cout << "\n서버와 연결이 종료되었습니다.\n";
            g_connected = false;
            break;
        } 

        buffer[received] = '\0';
        cout << "서버로부터 받은 메시지 : " << buffer << "\n";
    }
}
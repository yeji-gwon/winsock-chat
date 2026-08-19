#include "Config.h"
#include "ListenSocket.h"

bool g_running = true;

mutex g_coutMutex;              // 콘솔 출력 동기화

vector<CLIENT_INFO> g_clients;
mutex g_clientMutex;            // CLIENT_INFO 동기화
atomic<int> g_nextClientId = 0;

void LogMessage(const string& msg);
void AcceptLoop(const unique_ptr<ListenSocket>& listenSocket);
void AddClient(int id, shared_ptr<ClientSocket> clientSocket, const string& nickname);
void RemoveClient(int id);
size_t GetClientCount();
void BroadCast(const string& msg, int excludeId = -1);
void HandleClient(shared_ptr<ClientSocket> clientSocket);

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

    std::cout << "===================================================" << std::endl;
    std::cout << " 1:N 채팅 서버가 시작되었습니다. (포트: " << SERVER_PORT << ")" << std::endl;
    std::cout << " 콘솔에 문구를 입력하면 모든 클라이언트에게 공지로 전송됩니다." << std::endl;
    std::cout << " 접속자 목록 확인: /list   |   서버 종료: /quit" << std::endl;
    std::cout << "===================================================" << std::endl;

    thread acceptThread(AcceptLoop, ref(listenSocket));

    string line;
    while (getline(cin, line))
    {
        if (line == "/quit")
        {
            g_running = false;
            BroadCast("[공지] 서버가 종료됩니다.");
            break;
        }
        else if (line == "/list")
        {
            lock_guard<mutex> lock(g_clientMutex);
            LogMessage("-- 현재 접속자 (" + to_string(GetClientCount()) + "명) -- ");
            for (const auto& client : g_clients)
                LogMessage(client.nickname);

            continue;
        }
        else if (line.empty())
            continue;

        string noticeMsg = "[공지] " + line + "\n";
        BroadCast(noticeMsg);
        LogMessage("[공지 전송] " + noticeMsg);
    }

    {
        lock_guard<mutex> lock(g_clientMutex);
        for (auto& client : g_clients)
            client.socket->Shutdown();

        g_clients.clear();
    }
    listenSocket->Close();
    acceptThread.join();

    return 0;
}

void LogMessage(const string& msg)
{
    lock_guard<mutex> lock(g_coutMutex);
    cout << msg << endl;
}

void AcceptLoop(const unique_ptr<ListenSocket>& listenSocket)
{
    while (g_running)
    {
        auto clientSocket = listenSocket->Accept();

        if (!clientSocket)
        {
            cout << "[서버] accept 실패 : \n";
            break;
        } 
        thread(HandleClient, clientSocket).detach();
    }
}

void AddClient(int id, shared_ptr<ClientSocket> clientSocket, const string& nickname)
{
    lock_guard<mutex> lock(g_clientMutex);
    g_clients.push_back({ id, clientSocket, nickname });
}

void RemoveClient(int id)
{
    lock_guard<mutex> lock(g_clientMutex);
    g_clients.erase(remove_if(g_clients.begin(), g_clients.end(), [id](const CLIENT_INFO& info) {
        return info.id == id;}), g_clients.end());
}

size_t GetClientCount()
{
    lock_guard<mutex> lock(g_clientMutex);
    return g_clients.size();
}

void BroadCast(const string& msg, int excludeId)
{
    lock_guard<mutex> lock(g_clientMutex);
    for (const auto& client : g_clients)
    {
        if (client.id == excludeId)
            continue;

        client.socket->Send(msg.c_str(), static_cast<int>(msg.size()));
    }
}

void HandleClient(shared_ptr<ClientSocket> clientSocket)
{
    char buffer[BUF_SIZE];

    // accept 이후 첫 메시지는 닉네임
    int recvLen = clientSocket->Recv(buffer, BUF_SIZE - 1);
    if (recvLen <= 0)
        return;

    buffer[recvLen] = '\0';
    string nickname(buffer);
    if (nickname.empty())
        nickname = "익명";

    int id = g_nextClientId++;
    AddClient(id, clientSocket, nickname);

    string clientCount = to_string(GetClientCount());
    LogMessage("[서버] " + nickname + "님이 입장했습니다. (현재 접속자 수 : " + clientCount + "명)");

    string welcome = "[서버] " + nickname + "님, 환영합니다. (현재 접속자 수 : " + clientCount + "명)";
    clientSocket->Send(welcome.c_str(), static_cast<int>(welcome.size()));

    string joinMsg = "[알림] " + nickname + "님이 입장했습니다. (현재 접속자 수 : " + clientCount + "명)";
    BroadCast(joinMsg, id);

    while (g_running)
    { 
        recvLen = clientSocket->Recv(buffer, BUF_SIZE - 1);
        if (recvLen <= 0)
            break;

        buffer[recvLen] = '\0';
        string msg(buffer);
        if (msg.empty())
            continue;

        LogMessage("[" + nickname + "] " + msg);

        // 클라이언트 메시지를 보낸 사람 제외한 모두에게 전달
        string chatMsg = "[" + nickname + "] " + msg + "\n"; 
        BroadCast(chatMsg, id);
    }

    RemoveClient(id);

    clientCount = to_string(GetClientCount());
    LogMessage("[서버] " + nickname + "님이 퇴장했습니다. (현재 접속자 수 : " + clientCount + "명)");

    string exitMsg = "[알림] " + nickname + "님이 퇴장했습니다. (현재 접속자 수 : " + clientCount + "명)";
    BroadCast(exitMsg, id);
}
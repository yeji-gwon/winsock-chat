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

    unique_ptr<WinsockInit> winsockInit = WinsockInit::Create();
    if (!winsockInit)
        return 1;

    unique_ptr<ListenSocket> listenSocket = ListenSocket::Create();
    if (!listenSocket)
        return 1;

    if (!listenSocket->Bind(SERVER_PORT))
        return 1;

    return 0;
}
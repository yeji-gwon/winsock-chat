#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <iostream>
#include <memory>
#include "WinsockInit.h"
using namespace std;
 
int main()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    unique_ptr<WinsockInit> winsockInit = WinsockInit::Create();
    if (!winsockInit)
        return 1;

    return 0;
}
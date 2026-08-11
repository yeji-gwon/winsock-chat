#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <WinSock2.h> 
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

class ConnectSocket {
public:
	~ConnectSocket() { if(INVALID_SOCKET != m_sock) closesocket(m_sock); }

	// 소켓 핸들은 복사되면 이중 close 문제가 생기므로 복사 금지.
	ConnectSocket(const ConnectSocket&) = delete;
	ConnectSocket& operator= (const ConnectSocket&) = delete;

	// socket() 생성 실패 시 nullptr 반환.
	static std::unique_ptr<ConnectSocket> Create() {
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET)
		{
			std::cout << "socket() failed, error code : " << WSAGetLastError() << "\n";
			return nullptr;
		}

		return std::unique_ptr<ConnectSocket>(new ConnectSocket(sock));
	}

	bool Connect(const std::string& ip, unsigned short port) {
		SOCKADDR_IN addr = {};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);

		if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1)
		{
			std::cout << "inet_pton() failed\n";
			return false;
		}

		if (SOCKET_ERROR == connect(m_sock, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)))
		{
			std::cout << "connect() failed, error code : " << WSAGetLastError() << "\n";
			return false;
		}

		return true;
	}

	// 소켓 소유권을 포기하고 값만 반환. 이후 소멸자는 close를 호출하지 않음.
	SOCKET Release() {
		SOCKET sock = m_sock;
		m_sock = INVALID_SOCKET;
		return sock;
	}

private:
	ConnectSocket(SOCKET sock) : m_sock(sock) {}

	SOCKET m_sock;
};
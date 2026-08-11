#pragma once
#include <iostream>
#include <memory>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

class ListenSocket {
public:
	~ListenSocket() { closesocket(m_sock); }

	ListenSocket(const ListenSocket&) = delete;
	ListenSocket& operator= (const ListenSocket&) = delete;

	static std::unique_ptr<ListenSocket> Create() {
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET)
		{
			std::cout << "socket() failed, error code : " << WSAGetLastError() << "\n";
			return nullptr;
		}

		return std::unique_ptr<ListenSocket>(new ListenSocket(sock));
	}

	SOCKET Get() const { return m_sock; }

private:
	ListenSocket(SOCKET sock) : m_sock(sock) {}

	SOCKET m_sock;
};
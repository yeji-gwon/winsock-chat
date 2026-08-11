#pragma once
#include <iostream>
#include <memory>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

// accept()로 생성된 클라이언트 통신 소켓을 RAII로 관리하는 클래스.
// ListenSocket::Accept()에서만 생성되며, closesocket()은 소멸자가 자동으로 책임.

class ClientSocket {
public:
	~ClientSocket() { closesocket(m_sock); }

	// 소켓 핸들은 복사되면 이중 close 문제가 생기므로 복사 금지.
	ClientSocket(const ClientSocket&) = delete;
	ClientSocket& operator= (const ClientSocket&) = delete;

	// 유효성 검증이 끝난 소켓을 받아 RAII로 감쌈. (검증은 ListenSocket::Accept()의 책임)
	static std::unique_ptr<ClientSocket> Create(SOCKET sock) {		
		return std::unique_ptr<ClientSocket>(new ClientSocket(sock));
	}
	
	SOCKET Get() const { return m_sock;	}

private:
	ClientSocket(SOCKET sock) : m_sock(sock) {}

	SOCKET m_sock;
};
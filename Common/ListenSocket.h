#pragma once
#include <iostream>
#include <memory>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

// TCP 리스닝 소켓을 RAII로 관리하는 클래스.
// socket() 생성부터 closesocket()까지의 생명주기를 소멸자가 자동으로 책임.

class ListenSocket {
public:
	~ListenSocket() { closesocket(m_sock); }

	// 소켓 핸들은 복사되면 이중 close 문제가 생기므로 복사 금지.
	ListenSocket(const ListenSocket&) = delete;
	ListenSocket& operator= (const ListenSocket&) = delete;

	// socket() 생성 실패 시 nullptr 반환.
	static std::unique_ptr<ListenSocket> Create() {
		SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == INVALID_SOCKET)
		{
			std::cout << "socket() failed, error code : " << WSAGetLastError() << "\n";
			return nullptr;
		}

		return std::unique_ptr<ListenSocket>(new ListenSocket(sock));
	}

	// 리슨소켓 바인딩. IP 주소와 포트 번호 할당.
	bool Bind(unsigned short port) {
		SOCKADDR_IN addr = {};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 모든 네트워크 인터페이스로부터 연결
		addr.sin_port = htons(port);

		if (SOCKET_ERROR == bind(m_sock, reinterpret_cast<SOCKADDR*>(&addr), sizeof(addr)))
		{
			std::cout << "bind() failed, error code : " << WSAGetLastError() << "\n";
			return false;
		}

		return true;
	}

	// 소켓을 접속 대기 상태로 전환 (backlog는 기본값 SOMAXCONN 사용)
	bool Listen(int backlog = SOMAXCONN) {
		if (SOCKET_ERROR == listen(m_sock, backlog))
		{
			std::cout << "listen() failed, error code : " << WSAGetLastError() << "\n";
			return false;
		}

		return true;
	}

private:
	ListenSocket(SOCKET sock) : m_sock(sock) {}

	SOCKET m_sock;
};
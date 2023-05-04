#include "pch.h"
#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode" << errCode << endl;
}

const int32 BUFSIZE = 1000;

struct Session
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
	int32 sendBytes = 0;
};

int main()
{
	// WinSock 초기화 (ws2_3.lib 초기화), 관련 정보를 wsaData에 기록 192.168.0.48
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	// ----- 소캣 생성 -----
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET) // 논블로킹 소켓 설정
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	// ----- Bind -----
	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	// ----- Listen -----
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Listen!" << endl;

	// WSAEventSelect 모델
	// 

	vector<Session> sessions; // 현재 접속중인 Client 관리
	sessions.reserve(100);

	fd_set reads;
	fd_set writes;

	while (true)
	{
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		FD_SET(listenSocket, &reads);

		// Read할 차례인지 Write할 차례인지 판단 후 소켓 등록
		for (Session& s : sessions)
		{
			if (s.recvBytes <= s.sendBytes)
				FD_SET(s.socket, &reads);
			else
				FD_SET(s.socket, &writes);
		}

		// Read/Write 가능한 상태인 소켓만 남기고 제거
		int32 retVal = ::select(0, &reads, &writes, nullptr, nullptr);
		if (retVal == SOCKET_ERROR)
			break;

		if (FD_ISSET(listenSocket, &reads))
		{
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Accept!" << endl;

				sessions.push_back(Session{ clientSocket });
			}
		}

		for (Session& s : sessions)
		{
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen <= 0)
				{
					// sessions 제거
					continue;
				}

				s.recvBytes = recvLen;
			}

			if (FD_ISSET(s.socket, &writes))
			{
				// 일부 데이터만 보내는 경우 상정
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes], 
					s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR)
				{
					// sessions 제거
					continue;
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
			}
		}
	}


	::WSACleanup();
}

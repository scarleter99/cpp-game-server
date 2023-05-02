#include "pch.h"
#include <iostream>

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

void HandleError(const char* cause)
{
	int32 errCode = ::WSAGetLastError();
	cout << cause << " ErrorCode" << errCode << endl;
}

int main()
{
	// WinSock 초기화 (ws2_3.lib 초기화), 관련 정보를 wsaData에 기록 192.168.0.48
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData))
		return 0;

	// ----- 소캣 생성 -----
	SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(clientSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = ::htons(7777);

	// ----- Connect -----
	while (true)
	{
		if (::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK) // 블록했어야하는 경우
				continue;

			if (::WSAGetLastError() == WSAEISCONN) // 이미 연결된 상태인 경우
				break;

			break;
		}
	}

	cout << "Connect!" << endl;

	char sendBuffer[100] = "Hello World!";

	// ----- Send -----
	while (true)
	{
		if (::send(clientSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
		{
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;

			break;
		}

		cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;

		// ----- Recv -----
		while (true)
		{
			char recvBuffer[1000];
			int32 recvLen = ::recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
			if (recvLen == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSAEWOULDBLOCK)
					continue;

				break;
			}
			else if (recvLen == 0)
			{
				break; // 연결 끊김
			}

			cout << "Recv Data! Len = " << recvLen << endl;
			break;
		}

		this_thread::sleep_for(1s);
	}

	::closesocket(clientSocket);
	::WSACleanup();
}

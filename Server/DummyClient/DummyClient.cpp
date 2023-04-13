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
	// ad : Address Family (AF_INET = IPv4, AF_INET6 = IPv6)
	// type : TCP(SOCK_STREAM), UDP(SOCK_DGRAM)
	// protocol : 0
	SOCKET clientSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	// ----- 서버 주소 지정 -----
	SOCKADDR_IN serverAddr; // IPv4
	::memset(&serverAddr, 0, sizeof(serverAddr)); // 주소 초기화
	serverAddr.sin_family = AF_INET;
	// serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); // 구식
	::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // 신식
	serverAddr.sin_port = ::htons(7777);

	//::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)); // TCP처럼 Connect 가능

	// ----- Connect & Receive/Send -----
	this_thread::sleep_for(1s);

	while (true)
	{
		char sendBuffer[100] = "Hello World!";

		int32 resultCode = ::sendto(clientSocket, sendBuffer, sizeof(sendBuffer), 0, 
			(SOCKADDR*)&serverAddr, sizeof(serverAddr));
		if (resultCode == SOCKET_ERROR)
		{
			HandleError("Send");
			return 0;
		}

		cout << "Send Data! Len = " << sizeof(sendBuffer) << endl;

		SOCKADDR_IN recvAddr;
		::memset(&recvAddr, 0, sizeof(recvAddr));
		int32 addrLen = sizeof(recvAddr);

		char recvBuffer[1000];

		int32 recvLen = ::recvfrom(clientSocket, recvBuffer, sizeof(recvBuffer), 0,
			(SOCKADDR*)&recvAddr, &addrLen);
		if (recvLen <= 0)
		{
			HandleError("RecvFrom");
			return 0;
		}

		cout << "Recv Data! Data = " << recvBuffer << endl;
		cout << "Recv Data! Len = " << recvLen << endl;

		this_thread::sleep_for(1s);
	}

	::closesocket(clientSocket);
	::WSACleanup();
}

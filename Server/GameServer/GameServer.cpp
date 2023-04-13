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
	SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		HandleError("Socket");
		return 0;
	}

	// ----- setsockopt -----
	// s: 옵션 처리자
	// level: Socket(SOL_SOCKET), IPv4(IPPROTO_IP, TCP(IPPROTO_TCP)
	// optName: 연결확인(SO_KEEPALIVE), 지연(SO_LINGER)
	// *optVal:
	bool enable = true;
	::setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable, sizeof(enable));

	LINGER linger;
	linger.l_onoff = 1;
	linger.l_linger = 5; // 지연 시간
	::setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

	// ----- getsockopt -----
	// optName: 송신 버퍼 크기(SO_SNDBUF), 수신 버퍼 크기(SO_RCVBUF)
	int32 sendBufferSize;
	int32 optionLen = sizeof(sendBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, &optionLen);
	cout << "송신 버퍼 크기: " << sendBufferSize << endl;

	int32 recvBufferSize;
	int32 optionLen = sizeof(recvBufferSize);
	::getsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (char*)&recvBufferSize, &optionLen);
	cout << "수신 버퍼 크기: " << recvBufferSize << endl;

	// ----- shutdown(Half-Close) -----
	// how: send 통제(SD_SEND), recv 통제(SD_RECEIVE), 둘 다 통제(SD_BOTH)
	::shutdown(serverSocket, SD_SEND);



	::closesocket(serverSocket);

	::WSACleanup();
}

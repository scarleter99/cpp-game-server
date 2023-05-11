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
	// 소켓 관련 네트워크 이벤트를 이벤트 객체를 통해 감지
	// 생성: WSACreateEvent: 메뉴얼 리셋, 논시그널
	// 삭제: WSACloseEvent
	// 소켓, 이벤트 연동: WSAEventSelect(socket, event, networkEvents); - 논블로킹 모드 전환
	// 신호 상태 감지: WSAWaitForMultipleEvents: 64제한
	// 구체적인 네트워크 이벤트 확인: WSAEnumNetworkEvents
	// 이벤트: FD_ACCEPT(접속 클라 존재), FD_READ(수신 가능), FD_WRITE(송신 가능), 
	//		FD_CLOSE(상대 접속 종료), FD_CONNECT(통신 연결 완료), FD_OOB
	
	// ACCEPT()가 리턴한 소켓은 listenSocket과 동인한 속성
	// WSAWOULDBLOCK 에러 발생 가능
	// 이벤트 발생 후 소켓 함수를 호출하지 않으면, 동일한 다음 이벤트가 발생하지 않음

	

	vector<WSAEVENT> wsaEvents;
	vector<Session> sessions; // 현재 접속중인 Client 관리
	sessions.reserve(100);

	WSAEVENT listenEvent = ::WSACreateEvent();
	wsaEvents.push_back(listenEvent);
	sessions.push_back(Session{ listenSocket });
	if (::WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
		return 0;

	while (true)
	{
		// cEvents : event 수
		// *Events; event 객체들
		// waitAll: 전부 기다리는지, 하나만 완료되도 되는지
		// timeout
		// Alertable: FALSE
		// return : 완료된 첫번째 이벤트 idx
		int32 index = ::WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);
		if (index == WSA_WAIT_FAILED)
			continue;

		index -= WSA_WAIT_EVENT_0;

		WSANETWORKEVENTS networkEvents;
		// s: 소캣
		// eventObject: 해당 이벤트 객체 핸들과 연동된 이벤트 객체를 논시그널화
		// &networkEvent: 네트워크 이벤트 / 오류 정보 저장
		if (::WSAEnumNetworkEvents(sessions[index].socket, wsaEvents[index], &networkEvents) == SOCKET_ERROR)
			continue;

		// Listener 소켓 체크
		if (networkEvents.lNetworkEvents & FD_ACCEPT)
		{
			// 에러 체크
			if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
				continue;

			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);

			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Accept!" << endl;

				WSAEVENT clientEvent = ::WSACreateEvent();
				wsaEvents.push_back(clientEvent);
				sessions.push_back(Session{ clientSocket });
				if (::WSAEventSelect(clientSocket, clientEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
					return 0;
			}
		}

		// Client 소켓 체크
		if (networkEvents.lNetworkEvents & FD_READ || networkEvents.lNetworkEvents & FD_WRITE)
		{
			if ((networkEvents.lNetworkEvents & FD_READ) && (networkEvents.iErrorCode[FD_READ] != 0))
				continue;

			if ((networkEvents.lNetworkEvents & FD_WRITE) && (networkEvents.iErrorCode[FD_WRITE] != 0))
				continue;

			Session& s = sessions[index];

			if (s.recvBytes == 0)
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);
				if (recvLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// TODO : sessions 제거
					continue;
				}

				s.recvBytes = recvLen;
				cout << "Recv Data = " << recvLen << endl;
			}

			if (s.recvBytes > s.sendBytes)
			{
				// 일부 데이터만 보내는 경우 상정
				int32 sendLen = ::send(s.socket, &s.recvBuffer[s.sendBytes],
					s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR && ::WSAGetLastError() != WSAEWOULDBLOCK)
				{
					// TODO : sessions 제거
					continue;
				}

				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes)
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}

				cout << "Send Data = " << sendLen << endl;
			}
		}

		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			// TODO : sessions 제거
		}
	}

	::WSACleanup();
}

#pragma once

enum
{
	S_TEST = 1
};

class ClientPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len); // 패킷 처리

	static void Handle_S_TEST(BYTE* buffer, int32 len); // S_Test 패킷 처리
};


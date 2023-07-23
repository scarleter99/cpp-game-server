#pragma once

enum
{
	S_TEST = 1
};

struct BuffData
{
	uint64 buffId;
	float remainTime;
};

class ServerPacketHandler
{
public:
	static void HandlePacket(BYTE* buffer, int32 len); // 패킷 처리

	static SendBufferRef Make_S_TEST(uint64 id, uint32 hp, uint16 attack, vector<BuffData> buffs); // S_Test 패킷 생성
};


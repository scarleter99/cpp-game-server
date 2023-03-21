#pragma once

/*--------------
	1차 시도 : 멀티쓰레드 환경 부적합
--------------*/
/*
struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* header);
void PushEntyrSList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);
*/

/*--------------
	2차 시도 : ABA 문제, 접근중인 데이터 삭제 문제
--------------*/

/*
struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* header);
void PushEntyrSList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);
*/

/*--------------
	3차 시도 : 접근중인 데이터 삭제 문제
--------------*/

DECLSPEC_ALIGN(16)
struct SListEntry
{
	SListEntry* next;
};

DECLSPEC_ALIGN(16)
struct SListHeader
{
	SListHeader()
	{
		alignment = 0;
		region = 0;
	}

	union
	{
		struct
		{
			uint64 alignment;
			uint64 region;
		} DUMMYSTRUCTNAME;

		struct
		{
			uint64 depth : 16;
			uint64 sequence : 48;
			uint64 reserved : 4;
			uint64 next : 60;
		} HeaderX64; // depth, sequence는 alignment, reserved, next는 region에 대응
	};

	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* header);
void PushEntyrSList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);

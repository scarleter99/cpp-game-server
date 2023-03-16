#pragma once
#include <stack>
#include <map>
#include <vector>

/*--------------------
	DeadLockProfiler
---------------------*/

class DeadLockProfiler
{
public:
	void PushLock(const char* name);
	void PopLock(const char* name);
	void CheckCycle();

private:
	void Dfs(int32 index);

private:
	unordered_map<const char*, int32>	_nameToId; // Lock 사전
	unordered_map<int32, const char*>	_idToName; // Lock 사전
	stack<int32>						_lockStack;
	map<int32, set<int32>>				_lockHistory; // k : LockId, v : 해당 Lock 이후에 생성된 Lock

	Mutex _lock;

private:
	vector<int32>	_discoveredOrder;		// i : LockId, v : 해당 Lock이 발견된 순서
	int32			_discoveredCount = 0;	// 노드가 발견되 순서
	vector<bool>	_finished; // Dfs(i)	// i : LockId, v : 해당 Lock의 Dfs 종료 여부
	vector<int32>	_parent;				// i : LockId, v : 부모 Lock
};

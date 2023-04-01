#include "pch.h"

#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

using TL = TypeList<class Player, class Mage, class Knight, class Archer>;

class Player
{
	
public:
	Player()
	{
		INIT_TL(Player)
	}
	virtual ~Player() {}

	DECLARE_TL
};

class Knight :public Player
{
public:
	Knight() { INIT_TL(Knight) }
};

class Mage :public Player
{
public:
	Mage() { INIT_TL(Mage) }
};

class Archer :public Player
{
public:
	Archer() { INIT_TL(Archer) }
};

int main()
{
	Player* player1 = new Player();
	bool canCast1 = CanCast<Knight*>(player1); // false
	Knight* knight1 = TypeCast<Knight*>(player1);

	shared_ptr<Knight> knight2 = MakeShared<Knight>();
	bool canCast2 = CanCast<Knight>(knight2); // true
	shared_ptr<Player> player2 = TypeCast<Knight>(knight2);

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{

				}
			});
	}

	GThreadManager->Join();
}

#pragma once
#include "AntBehavior.h"

class Attack : public AntBehavior
{
public:

	Attack(Bot* bot) : AntBehavior(bot, attack_ants) {}
	void MakeMoves() override;
};


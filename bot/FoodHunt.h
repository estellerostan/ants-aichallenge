#pragma once
#include "AntBehavior.h"
#include "Bot.h"

class FoodHunt : public AntBehavior
{

public:

	FoodHunt(Bot* bot) : AntBehavior(bot, gather_food) {}
	void MakeMoves() override;
};


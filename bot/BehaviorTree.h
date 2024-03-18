#pragma once
#include "Behavior.h"

class BehaviorTree
{
	Behavior* root;
public:
	BehaviorTree();
	void tick();

};


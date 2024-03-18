#pragma once
#include "ActionStatus.h"

class Behavior
{
	ActionStatus status;

public:
	virtual ~Behavior() = default;
	Behavior();

	/**
	 * Called once BEFORE update
	 */
	virtual void onInitialize() {}

	/**
	 * Called once each time the BT is updated
	 */
	virtual ActionStatus update() = 0;

	/**
	 * Called once AFTER update
	 */
	virtual void onTerminate(ActionStatus) {}

	/**
	 * Wrapped for onInitialize, update and onTerminate sequence for ease of use
	 */
	ActionStatus tick();
};


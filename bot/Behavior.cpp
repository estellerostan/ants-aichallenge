#include "Behavior.h"

Behavior::Behavior() : status(DEFAULT)
{

}

ActionStatus Behavior::tick()
{
	if (status != RUNNING) onInitialize();
	status = update();
	if (status != RUNNING) onTerminate(status);
	return status;
}

// author: Changwan Yu
#include "pch.h"
#include "World.h"

bool pa::World::setDefaultMap(std::shared_ptr<Map> map)
{
	if (!map)
	{
		::DebugBreak();
		return false;
	}

	_map = std::move(map);
	return true;
}


#include "pch.h"
#include "AnimationManager.h"
#include "Skeleton.h"

pa::AnimationManager::AnimationManager()
{
}

void pa::AnimationManager::initialize()
{
}

pa::AnimationManager::~AnimationManager()
{
	if (nullptr != _skeleton)
		delete _skeleton;
}

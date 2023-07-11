#include "pch.h"
#include "Character.h"

pa::Character::Character()
{
	_skeleton = new Skeleton{};
}

pa::Character::~Character()
{
	delete _skeleton;
}

// author: Changwan Yu
#include "pch.h"
#include "Behavior.h"

void pa::Behavior::Composite::addChild(std::shared_ptr<Node> child)
{
	_children.push_back(std::move(child));
}

bool pa::Behavior::Sequence::onUpdate(World& world, std::weak_ptr<Actor> owner)
{
	for (std::shared_ptr<Node> child : _children)
	{
		const bool result = child->onUpdate(world, owner);
		if (false == result)
			return false;
	}

	return true;
}

bool pa::Behavior::Selector::onUpdate(World& world, std::weak_ptr<Actor> owner)
{
	for (std::shared_ptr<Node> child : _children)
	{
		const bool result = child->onUpdate(world, owner);
		if (true == result)
			return true;
	}

	return false;
}

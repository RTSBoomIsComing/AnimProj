// author: Changwan Yu
#include "pch.h"
#include "Behavior.h"

void pa::Behavior::Composite::addChild(std::shared_ptr<Node> child)
{
	_children.push_back(std::move(child));
}

bool pa::Behavior::Sequence::run()
{
	for (std::shared_ptr<Node> child : _children)
	{
		const bool result = child->run();
		if (false == result)
			return false;
	}

	return true;
}

bool pa::Behavior::Selector::run()
{
	for (std::shared_ptr<Node> child : _children)
	{
		const bool result = child->run();
		if (true == result)
			return true;
	}

	return false;
}

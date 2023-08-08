// author: Changwan Yu
#pragma once
#include "Behavior.h"
namespace pa
{
	class BehaviorTree
	{
	public:
		Behavior::Node* getRoot() { return _root.get(); }
	protected:
		std::shared_ptr<Behavior::Node> _root;
	};
}


// author: Changwan Yu
#pragma once
#include "BehaviorTree.h"
namespace pa
{
	class World;
	class Actor;
	class CharacterBehaviorTree : public BehaviorTree
	{
		class FindTarget : public Behavior::Node
		{
		public:
			virtual bool onUpdate(World& world, std::weak_ptr<Actor> owner) override;
			std::weak_ptr<Actor> _target;
		};

	public:
		CharacterBehaviorTree();
	};

}


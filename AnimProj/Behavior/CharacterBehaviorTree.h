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
			virtual bool onUpdate(World& world, Actor& owner) override;
		};

		class MoveToCenter : public Behavior::Node
		{
		public:
			virtual bool onUpdate(World& world, Actor& owner) override;
		};

	public:
		CharacterBehaviorTree();
	};

}

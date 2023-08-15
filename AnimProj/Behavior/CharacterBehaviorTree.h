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

		//class PlayAnimationLowerBody : public Behavior::Node
		//{
		//public:
		//	PlayAnimationLowerBody(class Animation const& animation)
		//		: _animation(&animation) {}
		//	virtual bool onUpdate(World& world, Actor& owner) override;

		//private:
		//	const Animation* _animation = nullptr;
		//};

		//class PlayAnimationUpperBody : public Behavior::Node
		//{
		//public:
		//	PlayAnimationUpperBody(class Animation const& animation)
		//		: _animation(&animation) {}
		//	virtual bool onUpdate(World& world, Actor& owner) override;

		//private:
		//	const Animation* _animation = nullptr;
		//};

	public:
		CharacterBehaviorTree();
	};

}

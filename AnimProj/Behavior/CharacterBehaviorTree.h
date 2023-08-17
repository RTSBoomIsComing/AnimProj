// author: Changwan Yu
#pragma once
#include "BehaviorTree.h"
namespace pa
{
	class World;
	class Actor;
	class CharacterBehaviorTree : public BehaviorTree
	{
		class FindTargetInAttackRange : public Behavior::Node
		{
		public:
			virtual bool onUpdate(World& world, Actor& owner) override;
		};

		class FindTargetInSight : public Behavior::Node
		{
		public:
			virtual bool onUpdate(World& world, Actor& owner) override;
		};

		class IsAttacking : public Behavior::Node
		{
		public:
			virtual bool onUpdate(World& world, Actor& owner) override;
		};

		class IsTargetInAttackRange : public Behavior::Node
		{
		public:
			virtual bool onUpdate(World& world, Actor& owner) override;
		};

		class Attack : public Behavior::Node
		{
		public:
			virtual bool onUpdate(World& world, Actor& owner) override;
		};

		class MoveToCenter : public Behavior::Node
		{
		public:
			virtual bool onUpdate(World& world, Actor& owner) override;

		private:
			DirectX::XMFLOAT3 _position;
		};

		class MoveAcrossMap : public Behavior::Node
		{
		public:
			virtual bool onUpdate(World& world, Actor& owner) override;
		};

	public:
		CharacterBehaviorTree(World& world);
	};
}

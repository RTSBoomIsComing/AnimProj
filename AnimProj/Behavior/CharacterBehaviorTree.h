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
			FindTarget(float radius)
				: _radius(radius) {}
			virtual bool onUpdate(World& world, Actor& owner) override;

		private:
			float _radius;
		};

		class CheckAttackRange : public Behavior::Node
		{
		public:
			CheckAttackRange(float radius)
				: _radius(radius) {}
			virtual bool onUpdate(World& world, Actor& owner) override;

		private:
			float _radius;
		};

	class MoveTo : public Behavior::Node
	{
	public:
		MoveTo(const DirectX::XMFLOAT3& position)
			: _position(position) {}
		virtual bool onUpdate(World& world, Actor& owner) override;

	private:
		DirectX::XMFLOAT3 _position;
	};

public:
	CharacterBehaviorTree(World& world);
};
}

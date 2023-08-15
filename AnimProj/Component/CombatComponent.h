// author: Changwan Yu
#pragma once
namespace pa
{
	class Actor;
	class World;
	class CombatComponent
	{
	public:
		CombatComponent() = default;

	public:
		void onUpdate(World& world, Actor& owner, float deltaTime);
		bool isAttacking() const { return false; }

	private:
		std::weak_ptr<Actor> _targetToAttack;
	};
}
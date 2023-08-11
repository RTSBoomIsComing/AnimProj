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
		void onUpdate(World& world, std::weak_ptr<Actor> owner, float deltaTime);

	private:
		std::weak_ptr<Actor> _targetToAttack;
	};
}
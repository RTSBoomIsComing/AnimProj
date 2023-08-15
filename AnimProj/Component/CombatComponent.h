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
		void onEndAttack();


		inline bool isAttacking() const { return _targetToAttack; }
		void setTargetToAttack(Actor* target);

	private:
		void applyDamage(World& world, Actor& owner);

	private:
		Actor*		_targetToAttack        = nullptr;
		bool 		_isAttacking		   = false;
		const float _attackPreparationTime = 2.7f;
		float		_attackTimer		   = 0.0f;
		const float _attackRange		   = 50.0f;
		const int	_attackDamage		   = 10;
		int			_health				   = 30;
	};
}
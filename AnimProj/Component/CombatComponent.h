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
		bool startAttack();
		void onAttack();
		void onEndAttack();

		inline bool	  isAttacking() const { return _isAttacking; }
		bool		  isTargetValid() const;
		inline float  getAttackRange() const { return _attackRange; }
		void		  setAttackRange(float attackRange) { _attackRange = attackRange; }
		void		  setTargetToAttack(Actor* target);
		inline Actor* getTargetToAttack() const { return _targetToAttack; }
		void		  setSightRange(float sightRange) { _sightRange = sightRange; }
		inline float  getSightRange() const { return _sightRange; }

	private:
		Actor*	  _targetToAttack = nullptr;
		bool	  _isAttacking	  = false;
		float	  _attackRange	  = 20.0f;
		const int _attackDamage	  = 10;
		int		  _health		  = 10;
		float	  _sightRange	  = 50.0f;
	};
}
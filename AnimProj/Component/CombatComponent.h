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

		bool		  isTargetValid() const;
		void		  setTargetToAttack(Actor* target);
		inline bool	  isAttacking() const { return _isAttacking; }
		inline float  getAttackRange() const { return _attackRange; }
		inline void	  setAttackRange(float attackRange) { _attackRange = attackRange; }
		inline Actor* getTargetToAttack() const { return _targetToAttack; }
		void		  setSightRange(float sightRange) { _sightRange = sightRange; }
		inline float  getSightRange() const { return _sightRange; }
		inline void	  setTeamID(int id) { _teamID = id; }
		inline int	  getTeamID() const { return _teamID; }

	private:
		Actor*	  _targetToAttack = nullptr;
		bool	  _isAttacking	  = false;
		float	  _attackRange	  = 20.0f;
		const int _attackDamage	  = 10;
		int		  _health		  = 20;
		float	  _sightRange	  = 40.0f;
		int		  _teamID		  = -1;
	};
}
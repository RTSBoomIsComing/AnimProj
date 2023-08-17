// author: Changwan Yu
#pragma once
#include "../World/World.h"
namespace pa
{
	class SceneComponent;
	class MovementComponent;
	class BehaviorTreeComponent;
	class SkeletalMeshComponent;
	class AnimationComponent2;
	class CombatComponent;

	template <typename... Ts>
	class IActor abstract
	{
	public:
		virtual void onStartGame(World& world) {}
		virtual void onUpdate(World& world, float deltaTime){};

	public:
		template <typename T>
		T* createDefaultComponent()
		{
			std::get<std::shared_ptr<T>>(_components) = std::make_shared<T>();
			return std::get<std::shared_ptr<T>>(_components).get();
		}

		template <typename T>
		T* getComponent()
		{
			return std::get<std::shared_ptr<T>>(_components).get();
		}

		template <typename T>
		const T* getComponent() const
		{
			return std::get<std::shared_ptr<T>>(_components).get();
		}

	private:
		std::tuple<std::shared_ptr<Ts>...> _components;
	};

	class Actor : public IActor<SceneComponent, BehaviorTreeComponent, MovementComponent, SkeletalMeshComponent, AnimationComponent2, CombatComponent>
	{
	public:
		inline void killThis() { _isAlive = false; }
		inline bool isAlive() const { return _isAlive; }
		inline bool isPendingKill() const { return _isPendingKill; }
		inline void onUpdate(World& world, float deltaTime)
		{
			if (_isPendingKill)
				return;
			if (!_isAlive)
			{
				_killTimer += deltaTime;
				if (_killTimer >= 3.0f)
				{
					_isPendingKill = true;
				}
			}
		}

	private:
		bool  _isAlive		 = true;
		bool  _isPendingKill = false;
		float _killTimer	 = 0.0f;
	};
}

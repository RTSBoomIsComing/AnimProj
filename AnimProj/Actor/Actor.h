// author: Changwan Yu
#pragma once
#include "../World/World.h"
#include "../Component/Component.h"
namespace pa
{
	class SceneComponent;
	class BehaviorTreeComponent;
	class MovementComponent;
	class SkeletalMeshComponent;

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

	class Actor : public IActor<SceneComponent, BehaviorTreeComponent, MovementComponent, SkeletalMeshComponent>
	{
	};
}

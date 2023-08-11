// author: Changwan Yu
#pragma once
#include "../World/World.h"
namespace pa
{
	class SceneComponent;
	class BehaviorTreeComponent;
	class MovementComponent;
	class SkeletalMeshComponent;
	class Actor abstract :  public std::enable_shared_from_this<Actor>
	{
	public:
		Actor()									= default;
		virtual ~Actor()						= default;

		Actor(Actor const& other)				= delete;
		Actor(Actor&& other)					= delete;
		Actor& operator=(Actor const& other)	= delete;
		Actor& operator=(Actor&& other)			= delete;

	public:
		virtual void initializeComponents(World& world)			{};
		virtual void onStartGame(World& world)					{};
		virtual void onUpdate(World& world, float deltaTime)	{};

	public:
		template<typename ComponentType>
		ComponentHandle<ComponentType> getComponentHandle() const;

		template<typename ComponentType>
		void setComponentHandle(ComponentHandle<ComponentType> handle);

		template<typename ComponentType>
		ComponentType& getComponent(World& world);

		template<typename ComponentType>
		void initializeComponent(World& world);


	protected:
		template<typename ComponentType>
		ComponentHandle<ComponentType>& getComponentHandleRef();

	protected:
		std::tuple<
			ComponentHandle<SceneComponent>,
			ComponentHandle<BehaviorTreeComponent>,
			ComponentHandle<MovementComponent>,
			ComponentHandle<SkeletalMeshComponent>> _componentHandles;
	};

	template<typename ComponentType>
	inline ComponentHandle<ComponentType> Actor::getComponentHandle() const
	{
		return std::get<ComponentHandle<ComponentType>>(_componentHandles);
	}

	template<typename ComponentType>
	inline void Actor::setComponentHandle(ComponentHandle<ComponentType> handle)
	{
		getComponentHandleRef<ComponentType>() = handle;
	}

	template<typename ComponentType>
	inline ComponentType& Actor::getComponent(World& world)
	{
		return world.getComponent(getComponentHandle<ComponentType>());
	}

	template<typename ComponentType>
	inline void Actor::initializeComponent(World& world)
	{
		setComponentHandle<ComponentType>(world.createComponent<ComponentType>(weak_from_this()));
	}

	template<typename ComponentType>
	inline ComponentHandle<ComponentType>& Actor::getComponentHandleRef()
	{
		return std::get<ComponentHandle<ComponentType>>(_componentHandles);
	}
}


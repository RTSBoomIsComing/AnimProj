// author: Changwan Yu
#pragma once
#include "../Component/Component.h"
namespace pa
{
	class World;
	class SceneComponent;
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
		virtual void initializeComponents(World& world)			abstract;
		virtual void onStartGame(World& world)					abstract;
		virtual void onUpdate(World& world, float deltaTime)	abstract;

	public:
		template<typename ComponentType>
		ComponentHandle<ComponentType> getComponentHandle() const;

		template<typename ComponentType>
		void setComponentHandle(ComponentHandle<ComponentType> handle);


	protected:
		template<typename ComponentType>
		ComponentHandle<ComponentType>& getComponentHandleRef();

	protected:
		std::tuple<ComponentHandle<SceneComponent>> _componentHandles;
	};

	template<typename ComponentType>
	inline ComponentHandle<ComponentType> Actor::getComponentHandle() const
	{
		return std::get<ComponentHandle<SceneComponent>>(_componentHandles);
	}

	template<typename ComponentType>
	inline void Actor::setComponentHandle(ComponentHandle<ComponentType> handle)
	{
		getComponentHandleRef<ComponentType>() = handle;
	}

	template<typename ComponentType>
	inline ComponentHandle<ComponentType>& Actor::getComponentHandleRef()
	{
		return std::get<ComponentHandle<SceneComponent>>(_componentHandles);
	}
}


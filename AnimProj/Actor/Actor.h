// author: Changwan Yu
#pragma once
namespace pa
{
	class World;
	class Actor abstract
	{
	public:
		Actor()									= default;
		virtual ~Actor()						= default;

		Actor(Actor const& other)				= delete;
		Actor(Actor&& other)					= delete;
		Actor& operator=(Actor const& other)	= delete;
		Actor& operator=(Actor&& other)			= delete;

		virtual void initializeComponents(World& world) abstract;
		virtual void onStartGame()						abstract;
		virtual void onUpdate(float deltaTime)			abstract;
	};
}


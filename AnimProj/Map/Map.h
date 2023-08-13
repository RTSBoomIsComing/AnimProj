// author: Changwan Yu
#pragma once
namespace pa
{
	class World;
	class Actor;
	class Map abstract
	{
	public:
		Map()			= default;
		virtual ~Map()	= default;

	public:
		virtual bool placeActor(World& world, Actor& actor) { return false; }
		virtual void clearMap() abstract;
	};
}


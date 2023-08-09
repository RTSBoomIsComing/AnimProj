// author: Changwan Yu
#pragma once
namespace pa
{
	class Character;
	class Map abstract
	{
	public:
		Map()			= default;
		virtual ~Map()	= default;

	public:
		virtual bool placeObject(Character* object) abstract;
	};
}


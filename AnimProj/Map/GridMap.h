// author: Changwan Yu
#pragma once
#include "Map.h"
#include "../Component/SceneComponent.h"
namespace pa
{
	class World;
	class GridMap : public Map
	{
	public:
		GridMap(float cellSize) : _cellSize(cellSize) {}
		~GridMap()	= default;

	public:
		virtual bool placeActor(World& world, Actor& actor) override;
		virtual void clearMap() override;

		std::pair<size_t, size_t> getCellCoordinate(World& world, Actor& actor) const;
		std::vector<Actor*>& getCell(size_t x, size_t y);

	private:
		static constexpr size_t	_mapSize	= 40;
		float					_cellSize	= 1.0f;
		std::array<std::array<std::vector<Actor*>, _mapSize>, _mapSize> _cells;
	};
}


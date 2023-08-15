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
		std::vector<Actor*>& getCell(size_t x, size_t z);
		std::pair<float, float> getCellCenter(size_t x, size_t z) const;

		size_t getMapWidth() const { return _mapWidth; }
		size_t getMapHeight() const { return _mapHeight; }

	private:
		static constexpr size_t	_mapWidth	= 10;
		static constexpr size_t	_mapHeight	= 10;

		float					_cellSize	= 1.0f;
		std::array<std::array<std::vector<Actor*>, _mapWidth>, _mapHeight> _cells;
	};
}


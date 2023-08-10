// author: Changwan Yu
#pragma once
#include "Map.h"
#include "../Component/Component.h"
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
		virtual bool placeActor(World& world, std::weak_ptr<Actor> actor) override;
		virtual void clearMap() override;

		std::pair<size_t, size_t> getCellCoordinate(World& world, std::weak_ptr<Actor> actor) const;
		std::vector<std::weak_ptr<Actor>>& getCell(size_t x, size_t y);

	private:
		static constexpr size_t	_mapSize	= 40;
		float					_cellSize	= 1.0f;
		std::array<std::array<std::vector<std::weak_ptr<Actor>>, _mapSize>, _mapSize> _cells;
	};
}


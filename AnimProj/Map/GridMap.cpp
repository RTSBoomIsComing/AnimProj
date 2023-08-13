// author: Changwan Yu
#include "pch.h"
#include "GridMap.h"
#include "../Actor/Actor.h"
#include "../Component/SceneComponent.h"

namespace pa
{

	bool GridMap::placeActor(World& world, Actor& actor)
	{
		std::pair<size_t, size_t> cellXZ = getCellCoordinate(world, actor);
		if (cellXZ.first < 0 || cellXZ.second < 0 || cellXZ.first >= _mapSize || cellXZ.second >= _mapSize)
			return false;

		getCell(cellXZ.first, cellXZ.second).push_back(&actor);
		return true;
	}

	void GridMap::clearMap()
	{
		for (auto& cellRow : _cells)
		{
			for (auto& cell : cellRow)
			{
				cell.resize(0);
			}
		}
	}

	std::pair<size_t, size_t> GridMap::getCellCoordinate(World& world, Actor& actor) const
	{
		const SceneComponent* sceneComp = actor.getComponent<SceneComponent>();
		size_t				  cellX		= static_cast<size_t>(sceneComp->position.x / _cellSize + _mapSize * 0.5);
		size_t				  cellZ		= static_cast<size_t>(sceneComp->position.z / _cellSize + _mapSize * 0.5);

		assert(cellX < _mapSize && cellZ < _mapSize);
		return std::pair<size_t, size_t>(cellX, cellZ);
	}

	std::vector<Actor*>& GridMap::getCell(size_t x, size_t y)
	{
		assert(x < _mapSize && y < _mapSize);
		return _cells[x][y];
	}

}
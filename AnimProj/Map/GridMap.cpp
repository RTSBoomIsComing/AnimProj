// author: Changwan Yu
#include "pch.h"
#include "GridMap.h"
#include "../Actor/Actor.h"
#include "../Component/SceneComponent.h"
bool pa::GridMap::placeActor(World& world, std::weak_ptr<Actor> actor)
{
	std::pair<size_t, size_t> cellXZ = getCellCoordinate(world, actor);
	_cells[cellXZ.first][cellXZ.second].push_back(actor);

	return true;
}

void pa::GridMap::clearMap()
{
	for (auto& cellRow : _cells)
	{
		for (auto& cell : cellRow)
		{
			cell.resize(0);
		}
	}
}

std::pair<size_t, size_t> pa::GridMap::getCellCoordinate(World& world, std::weak_ptr<Actor> actor) const
{
	SceneComponent sceneComponent = actor.lock()->getComponent<SceneComponent>(world);
	size_t cellX = sceneComponent.position.x / _cellSize + _mapSize * 0.5;
	size_t cellZ = sceneComponent.position.z / _cellSize + _mapSize * 0.5;

	return std::pair<size_t, size_t>(cellX, cellZ);
}

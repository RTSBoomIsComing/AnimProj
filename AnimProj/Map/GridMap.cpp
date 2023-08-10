// author: Changwan Yu
#include "pch.h"
#include "GridMap.h"
#include "../Actor/Actor.h"
#include "../Component/SceneComponent.h"
bool pa::GridMap::placeActor(World& world, std::weak_ptr<Actor> actor)
{
	SceneComponent sceneComponent = actor.lock()->getComponent<SceneComponent>(world);
	size_t cellX = sceneComponent.position.x / _cellSize + _mapSize * 0.5;
	size_t cellZ = sceneComponent.position.z / _cellSize + _mapSize * 0.5;

	_cells[cellX][cellZ].push_back(actor);

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

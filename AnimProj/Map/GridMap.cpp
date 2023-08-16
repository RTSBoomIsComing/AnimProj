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
		if (cellXZ.first < 0 || cellXZ.second < 0 || cellXZ.first >= _mapWidth || cellXZ.second >= _mapHeight)
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
		assert(sceneComp);

		size_t cellX = static_cast<size_t>(sceneComp->position.x / _cellSize);
		size_t cellZ = static_cast<size_t>(sceneComp->position.z / _cellSize);

		assert(0 <= cellX && 0 <= cellZ && cellX < _mapWidth && cellZ < _mapHeight);
		return std::pair<size_t, size_t>(cellX, cellZ);
	}

	std::vector<Actor*>& GridMap::getCell(size_t x, size_t z)
	{
		assert(x < _mapWidth && z < _mapHeight);
		return _cells[x][z];
	}

	std::pair<float, float> GridMap::getCellCenter(size_t x, size_t z) const
	{
		if (x < _mapWidth && z < _mapHeight)
		{
			return std::pair<float, float>(
				(x + 0.5f) * _cellSize,
				(z + 0.5f) * _cellSize);
		}

		DebugBreak();
		return std::pair<float, float>(-1.0f, -1.0f);
	}

	Actor* GridMap::findNearestActor(World& world, Actor& actor, float radius) const
	{
		using namespace DirectX;

		const SceneComponent* sceneComp = actor.getComponent<SceneComponent>();
		assert(sceneComp);

		int startX = static_cast<int>((sceneComp->position.x - radius) / _cellSize);
		int endX   = static_cast<int>((sceneComp->position.x + radius) / _cellSize);
		int startZ = static_cast<int>((sceneComp->position.z - radius) / _cellSize);
		int endZ   = static_cast<int>((sceneComp->position.z + radius) / _cellSize);

		startX = startX > 0 ? startX : 0;
		startZ = startZ > 0 ? startZ : 0;
		endX   = endX < _mapWidth ? endX : _mapWidth;
		endZ   = endZ < _mapHeight ? endZ : _mapHeight;

		float  minDistance	= std::numeric_limits<float>::max();
		Actor* nearestActor = nullptr;

		XMVECTOR V0 = XMLoadFloat3(&sceneComp->position);
		for (size_t x = startX; x < endX; ++x)
		{
			for (size_t z = startZ; z < endZ; ++z)
			{
				for (Actor* other : _cells[x][z])
				{
					if (&actor == other)
						continue;

					const SceneComponent* otherSceneComp = other->getComponent<SceneComponent>();
					assert(otherSceneComp);

					XMVECTOR V1		  = XMLoadFloat3(&otherSceneComp->position);
					float	 distance = XMVectorGetX(XMVector3Length(V1 - V0));

					if (distance < minDistance)
					{
						minDistance	 = distance;
						nearestActor = other;
					}
				}
			}
		}

		return nearestActor;
	}

}
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
		GridMap(float cellSize)
			: _cellSize(cellSize) {}
		~GridMap() = default;

	public:
		virtual bool placeActor(World& world, Actor& actor) override;
		virtual void clearMap() override;

		std::pair<size_t, size_t> getCellCoordinate(World& world, Actor& actor) const;
		std::vector<Actor*>&	  getCell(size_t x, size_t z);
		std::pair<float, float>	  getCellCenter(size_t x, size_t z) const;

		inline size_t				   getMapWidth() const { return _mapWidth; }
		inline size_t				   getMapHeight() const { return _mapHeight; }
		inline float				   getCellSize() const { return _cellSize; }
		inline std::pair<float, float> getMapCenter() const { return std::make_pair(_mapWidth * _cellSize / 2.0f, _mapHeight * _cellSize / 2.0f); }

		Actor* findNearestActor(World& world, Actor& actor, float radius) const;

	private:
		static constexpr size_t _mapWidth  = 10;
		static constexpr size_t _mapHeight = 10;
		float					_cellSize  = 1.0f;

		std::array<std::array<std::vector<Actor*>, _mapWidth>, _mapHeight> _cells;
	};
}

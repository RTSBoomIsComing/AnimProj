// author: Changwan Yu
#pragma once

namespace pa
{
	class GridMap;
	class Actor;
	class World final
	{
	public:
		World();
		~World();
	public:
		void startGame();
		void update(float deltaTime);

	public:
		inline std::shared_ptr<GridMap> getDefaultMap() { return _map; }
		std::vector<DirectX::XMFLOAT4X4> boneMatrixPool;
		std::vector<DirectX::XMFLOAT4X4> boneToBoneMatrixPool;

	private:
		std::shared_ptr<GridMap>					_map;
		std::vector<std::shared_ptr<Actor>>			_actors;
	};
}


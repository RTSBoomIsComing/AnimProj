// author: Changwan Yu
#pragma once

namespace pa
{
	class GridMap;
	class Actor;
	class Camera;
	class World final
	{
	public:
		World();
		~World();

	public:
		void startGame();
		void update(float deltaTime);

		inline void						setDefaultCamera(const Camera* camera) { _camera = camera; }
		inline std::shared_ptr<GridMap> getDefaultMap() { return _map; }

	public:
		std::vector<DirectX::XMFLOAT4X4> boneMatrixPool;
		std::vector<DirectX::XMFLOAT4X4> boneToBoneMatrixPool;

	private:
		void cullActor(Actor* actor);

	private:
		const Camera*						_camera = nullptr;
		std::shared_ptr<GridMap>			_map;
		std::vector<std::shared_ptr<Actor>> _actors;
	};
}

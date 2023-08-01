// author: Changwan Yu
#pragma once
#include "Mesh.h"
#include "CubeMesh.h"
#include "StickMesh.h"
namespace pa
{
	class MeshManager
	{
	public:
		MeshManager();
		~MeshManager() = default;

		static MeshManager& get()
		{
			static MeshManager meshManager;
			return meshManager;
		}

	public:
		void initialize(ID3D11Device* device);
		Mesh const& getDefaultCube() const { return cubeList.front(); }
		Mesh const& getDefaultStick() const { return stickList.front(); }

	private:
		std::vector<CubeMesh> cubeList;
		std::vector<StickMesh> stickList;
	};
}


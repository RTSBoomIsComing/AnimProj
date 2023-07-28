#include "pch.h"
#include "MeshManager.h"

pa::MeshManager::MeshManager()
{
}

void pa::MeshManager::initialize(ID3D11Device* device)
{
	cubeList.push_back(CubeMesh(device, 0.25f));
	stickList.push_back(CubeMesh(device));
}

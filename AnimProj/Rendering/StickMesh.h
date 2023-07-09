#pragma once
#include "Mesh.h"
namespace pa
{
	class StickMesh : public pa::Mesh
	{
	public:
		StickMesh(ID3D11Device* pDevice);
		~StickMesh() = default;
	};
}

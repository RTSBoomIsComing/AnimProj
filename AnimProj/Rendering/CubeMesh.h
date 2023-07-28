#pragma once
#include "Mesh.h"

namespace pa
{
	class CubeMesh : public Mesh
	{
	public:
		CubeMesh(ID3D11Device* pDevice, float scale = 1.0f);
		~CubeMesh() = default;
	};
}

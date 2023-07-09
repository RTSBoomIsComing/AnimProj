#pragma once
#include "Mesh.h"

namespace pa
{
	class CubeMesh : public pa::Mesh
	{
	public:
		CubeMesh(ID3D11Device* pDevice);
		~CubeMesh() = default;
	};
}

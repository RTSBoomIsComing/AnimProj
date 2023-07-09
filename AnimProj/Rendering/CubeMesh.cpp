#include "pch.h"
#include "CubeMesh.h"

pa::CubeMesh::CubeMesh(ID3D11Device* pDevice, float scale)
{
	using namespace DirectX;

	scale *= 0.5f;
	std::vector<XMFLOAT4> positions =
	{
		{ -scale, -scale, -scale, 1.0 }, //0
		{ -scale,  scale, -scale, 1.0 }, //1
		{  scale,  scale, -scale, 1.0 }, //2
		{  scale, -scale, -scale, 1.0 }, //3
		{ -scale, -scale,  scale, 1.0 }, //4
		{ -scale,  scale,  scale, 1.0 }, //5
		{  scale,  scale,  scale, 1.0 }, //6
		{  scale, -scale,  scale, 1.0 }, //7
	};

	std::vector<XMFLOAT4> colors =
	{
		{ 0,  0,  1, 1 }, //0
		{ 1,  0,  0, 1 }, //1
		{ 1,  0,  0, 1 }, //2
		{ 0,  0,  1, 1 }, //3
		{ 0,  0,  1, 1 }, //4
		{ 1,  0,  0, 1 }, //5
		{ 1,  0,  0, 1 }, //6
		{ 0,  0,  1, 1 }, //7
	};

	//std::vector<XMFLOAT4> colors =
	//{
	//	{ 1,  0,  0, 1 }, //0
	//	{ 1,  0,  0, 1 }, //1
	//	{ 1,  0,  0, 1 }, //2
	//	{ 1,  0,  0, 1 }, //3
	//	{ 1,  0,  0, 1 }, //4
	//	{ 1,  0,  0, 1 }, //5
	//	{ 1,  0,  0, 1 }, //6
	//	{ 1,  0,  0, 1 }, //7
	//};

	processVertices(pDevice, positions, colors);

	// Supply the actual index data.
	std::vector<UINT> indices = {
		//Front
		0, 1, 2,
		0, 2, 3,

		//Back
		7, 6, 5,
		7, 5, 4,

		//Top
		1, 5, 6,
		1, 6, 2,

		//Bottom
		3, 7, 4,
		3, 4, 0,

		//Left
		3, 2, 6,
		3, 6, 7,

		//Right
		4, 5, 1,
		4, 1, 0,
	};

	processIndices(pDevice, indices);
}

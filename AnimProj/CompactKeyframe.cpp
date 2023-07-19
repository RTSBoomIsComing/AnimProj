// author: Changwan Yu
#include "pch.h"
#include "CompactKeyframe.h"

pa::CompactKeyframe pa::CompactKeyframe::createFromQuaternion(DirectX::XMVECTOR Q)
{
	using namespace DirectX;
	using namespace DirectX::PackedVector;
	float v[4] = {};

	v[0] = XMVectorGetX(Q);
	v[1] = XMVectorGetY(Q);
	v[2] = XMVectorGetZ(Q);
	v[3] = XMVectorGetW(Q);
	
	int		maxIndex	= -1;
	bool	maxSign		= false;
	float	maxAbsValue	= -1.0f;

	for (int i = 0; i < 4; i++)
	{
		float fabs = std::fabs(v[i]);
		if (maxAbsValue < fabs)
		{
			maxAbsValue = fabs;
			maxIndex	= i;
			maxSign		= (v[i] < 0.0f);
		}
	}

	assert(maxIndex > -1);
	
	// 1 / sqrt(2)
	constexpr float		sqrt1_2 = 0.707106781186547524401f;
	// from	: -1/sqrt2	<= X <=	1/sqrt2
	// to	: -1/2		<= X <=	1/2
	Q = Q * sqrt1_2;

	// from : -1/2		<= X <=	1/2
	// to	: 0			<= X <= 1
	Q = Q + XMVectorReplicate(0.5f);
	Q = XMVectorSaturate(Q);
	
	// from	: 0			<= X <= 1
	// to	: 0			<= X <= 65535
	Q = Q * std::numeric_limits<uint16_t>::max();

	CompactKeyframe keyframe = {};
	XMStoreUShort4(&keyframe.asUShort4, Q);

	int needShift = 0;
	for (int i = 0; i < 3; i++)
	{
		if (maxIndex == i)
			needShift = 1;

		keyframe.v[i] = keyframe.v[i + needShift];
	}
	
	keyframe.v[3] = 0;
	keyframe.opt_0 = static_cast<uint16_t>(maxSign);
	keyframe.opt_1 = static_cast<uint16_t>(maxIndex);



	return keyframe;
}

pa::CompactKeyframe pa::CompactKeyframe::createFromVector(DirectX::XMVECTOR V)
{
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	CompactKeyframe keyframe = {};
	XMStoreHalf4(&keyframe.asHalf4, V);
	keyframe.v[3] = 0;

	return keyframe;
}

DirectX::XMVECTOR pa::CompactKeyframe::decompressAsVector() const
{
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	XMVECTOR V = XMLoadHalf4(&this->asHalf4);
	XMVectorSetW(V, 0.0f);

	return V;
}

//DirectX::XMVECTOR pa::CompactKeyframe::decompressAsQuaternion() const
//{
//	using namespace DirectX;
//	using namespace DirectX::PackedVector;
//
//	XMVECTOR V = XMLoadHalf4(&this->asHalf4);
//	XMVectorSetW(V, 0.0f);
//
//	int needShift = 0;
//	for (int i = 0; i < 3; i++)
//	{
//		if (maxIndex == i)
//			needShift = 1;
//
//		keyframe.v[i] = keyframe.v[i + needShift];
//	}
//
//	keyframe.v[3] = 0;
//	keyframe.opt_0 = static_cast<uint16_t>(maxSign);
//	keyframe.opt_1 = static_cast<uint16_t>(maxIndex);
//
//	return V;
//
//}

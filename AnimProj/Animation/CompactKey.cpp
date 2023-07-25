// author: Changwan Yu
#include "pch.h"
#include "CompactKey.h"

// max compression error is 0.000021579
// 1 * sqrt(2) / 65535 = 0.000021579
pa::CompactKey pa::CompactKey::createFromQuaternion(DirectX::XMVECTOR Q)
{
	using namespace DirectX;
	using namespace DirectX::PackedVector;
	XMFLOAT4 elements = {};
	XMStoreFloat4(&elements, Q);
	
	int		biggestPosition	= -1;
	bool	biggestSign		= false;
	float	biggestValue	= -1.0f;

	for (int i = 0; i < 4; i++)
	{
		const float element	= (&elements.x)[i];
		const float fabs	= std::fabs(element);
		if (biggestValue < fabs)
		{
			biggestValue = fabs;
			biggestPosition	= i;
			biggestSign		= (element < 0.0f);
		}
	}
	
	// from	: -1/sqrt(2)<= X <=	1/sqrt(2)
	// to	: -1/2		<= X <=	1/2
	constexpr float		sqrt1_2 = 0.707106781186547524401f; /* 1/sqrt(2) */
	Q = Q * sqrt1_2;

	// from : -1/2		<= X <=	1/2
	// to	: 0			<= X <= 1
	Q = Q + XMVectorReplicate(0.5f);
	Q = XMVectorSaturate(Q);
	
	// from	: 0			<= X <= 1
	// to	: 0			<= X <= 65535
	Q = Q * std::numeric_limits<uint16_t>::max();

	switch (biggestPosition)
	{
	case 3:
		// Do nothing
		// Q = XMVectorSwizzle<0, 1, 2, 3>(Q);
		break;
	case 0:
		Q = XMVectorSwizzle<3, 1, 2, 0>(Q);
		break;
	case 1:
		Q = XMVectorSwizzle<0, 3, 2, 1>(Q);
		break;	
	case 2:
		Q = XMVectorSwizzle<0, 1, 3, 2>(Q);
		break;
	}

	Q = XMVectorSetW(Q, 0.0f);

	CompactKey keyframe = {};
	XMStoreUShort4(&keyframe.asUShort4, Q);

	keyframe.opt_0 = static_cast<uint16_t>(biggestSign);
	keyframe.opt_1 = static_cast<uint16_t>(biggestPosition);

	return keyframe;
}

pa::CompactKey pa::CompactKey::createFromVector(DirectX::XMVECTOR V)
{
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	CompactKey keyframe = {};
	XMStoreHalf4(&keyframe.asHalf4, V);
	keyframe.v[3] = 0;

	return keyframe;
}

// max compression error is 0.000021579
// 1 * sqrt(2) / 65535 = 0.000021579
DirectX::XMVECTOR pa::CompactKey::decompressAsQuaternion() const
{
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	const uint16_t biggestSign		= this->opt_0;
	const uint16_t biggestPosition	= this->opt_1;


	XMVECTOR Q = XMLoadUShort4(&this->asUShort4);

	// from	: 0			<= X <= 65535
	// to  	: 0			<= X <= 1
	Q = Q / std::numeric_limits<uint16_t>::max();

	// from : 0			<= X <= 1
	// to	: -1/2		<= X <=	1/2
	Q = Q - XMVectorReplicate(0.5f);

	// from	: -1/2		<= X <=	1/2
	// to	: -1/sqrt(2)<= X <=	1/sqrt(2) 
	// NOTE : 1/sqrt(2)	== sqrt(2)/2
	constexpr float		sqrt2 = 1.41421356237309504880f;
	Q = Q * sqrt2;

	// Can skip this
	// XMVector3LengthSq only use X,Y,Z components
	//Q = XMVectorSetW(Q, 0.0f);

	const float lengthSq = XMVectorGetX(XMVector3LengthSq(Q));
	float biggest = 1 - lengthSq;
	biggest = std::sqrtf(biggest);

	if (biggestSign)
		biggest = biggest * -1;

	Q = XMVectorSetW(Q, biggest);

	switch (biggestPosition)
	{
	case 3:
		// Do nothing
		// Q = XMVectorSwizzle<0, 1, 2, 3>(Q);
		break;
	case 0:
		Q = XMVectorSwizzle<3, 1, 2, 0>(Q);
		break;
	case 1:
		Q = XMVectorSwizzle<0, 3, 2, 1>(Q);
		break;
	case 2:
		Q = XMVectorSwizzle<0, 1, 3, 2>(Q);
		break;
	}

	return Q;
}

DirectX::XMVECTOR pa::CompactKey::decompressAsVector() const
{
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	XMVECTOR V = XMLoadHalf4(&this->asHalf4);
	V = XMVectorSetW(V, 0.0f);

	return V;
}
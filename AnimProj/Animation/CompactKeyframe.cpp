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

DirectX::XMVECTOR pa::CompactKeyframe::decompressAsQuaternion() const
{
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	uint16_t maxSign	= this->opt_0;
	uint16_t maxIndex	= this->opt_1;


	XMVECTOR Q = XMLoadUShort4(&this->asUShort4);

	// from	: 0			<= X <= 65535
	// to  	: 0			<= X <= 1
	Q = Q / std::numeric_limits<uint16_t>::max();

	// from : 0			<= X <= 1
	// to	: -1/2		<= X <=	1/2
	Q = Q - XMVectorReplicate(0.5f);

	// sqrt(2)
	constexpr float		sqrt2 = 1.41421356237309504880f;

	// from	: -1/2		<= X <=	1/2
	// to	: -1/sqrt2	<= X <=	1/sqrt2 
	// NOTE : 1/sqrt2 == sqrt2 / 2
	Q = Q * sqrt2;

	XMVectorSetW(Q, 0.0f);

	const float lengthSq = XMVectorGetX(XMVector3LengthSq(Q));
	float biggest = 1 - lengthSq;
	biggest = std::sqrtf(biggest);

	if (maxSign)
		biggest = biggest * -1;

	XMFLOAT4 decompressed = {};
	XMStoreFloat4(&decompressed, Q);

	float *pDecompressed = &decompressed.x;

	for (int i = 3; 0 <= i; i--)
	{
		if (maxIndex == i)
		{
			pDecompressed[i] = biggest;
			break;
		}

		pDecompressed[i] = pDecompressed[i - 1];
	}

	return XMLoadFloat4(&decompressed);
}

DirectX::XMVECTOR pa::CompactKeyframe::decompressAsVector() const
{
	using namespace DirectX;
	using namespace DirectX::PackedVector;

	XMVECTOR V = XMLoadHalf4(&this->asHalf4);
	XMVectorSetW(V, 0.0f);

	return V;
}
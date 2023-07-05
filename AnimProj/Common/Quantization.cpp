#include "pch.h"
#include "Quantization.h"

pa::Quantization::Quantization(const DirectX::XMFLOAT4& quaternion)
	: _data(0)
{
	using namespace DirectX;
	constexpr float		sqrt1_2 = 0.707106781186547524401; // 1 / sqrt(2)

	size_t				discard = 0;
	size_t				index = 0;

	const float* floats = &quaternion.x;

	for (size_t i{}; i < 4; i++)
	{
		if (std::fabs(floats[i]) <= sqrt1_2)
			_data |= static_cast<uint64_t>(quantizeFloat(floats[i])) << (44 * (-15 * index++));
		else
			discard = i;
	}

	_data += discard << 46;

	if (floats[discard] < 0)
	{
		for (size_t i{}; i < 3; i++)
		{
			_data ^= 1ui64 << (44 * (-15 * i));
		}
	}
}

DirectX::XMVECTOR pa::Quantization::deQuantize()
{

	return DirectX::XMVECTOR();
}

int16_t pa::Quantization::quantizeFloat(float value) const
{
	constexpr int16_t	maxQuantizeValue = 16383;			// 2^14 - 1;
	constexpr float		sqrt2 = 1.41421356237309504880f;	// sqrt(2)

	// NOTE
	// if the value is sqrt(2) then result is 16383
	// guarantees a numerical precision of 0.000043
	// that value has been acquired from 
	// 1 /(16383 * sqrt(2)) = 4.3161007213974700872907548196597e-5

	return   maxQuantizeValue * (sqrt2 * value);
}

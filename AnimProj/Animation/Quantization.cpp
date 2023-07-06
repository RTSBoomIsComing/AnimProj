#include "pch.h"
#include "Quantization.h"

DirectX::XMVECTOR pa::Quantization::deQuantize(uint64_t quantized)
{
	using namespace DirectX;
	size_t				index		= 0;
	float				floats[4]	= {};

	const bool			discardSign	= (quantized >> 47) & 0x1;
	const size_t		discard		= (quantized >> 45) & 0x3;
	const float			squareSum	= XMVectorGetX(XMVectorSum(XMVector4LengthSq(XMLoadFloat(floats))));

	for (size_t i = 0; i < 4; i++)
	{
		if (discard == i)
			continue;

		floats[i] = deQuantizeFloat(quantized, index++);
	}

	floats[discard] = std::sqrtf(1.0f - squareSum) * (discardSign ? -1 : 1);

	return XMLoadFloat(floats);
}

uint64_t pa::Quantization::quantize(const DirectX::XMFLOAT4& vector4)
{
	using namespace DirectX;
	constexpr float		sqrt1_2 = 0.707106781186547524401f; // 1 / sqrt(2)

	const float*		floats = &vector4.x;
	size_t				discard = std::numeric_limits<size_t>::max();
	float				max = 0.0f;

	// find index to be discarded
	for (size_t i = 0; i < 4; i++)
	{
		if (std::fabs(floats[i] > max))
		{
			discard = i;
			max = floats[i];
		}
	}

	assert(discard < 4 && "there was no discard");


	uint64_t	quantized	= 0;
	size_t		index		= 0;

	for (size_t i = 0; i < 4; i++)
	{
		if (discard == i)
			continue;
	
		quantized |= static_cast<uint64_t>(quantizeFloat(floats[i])) << (30 * (-15 * index++));	// 30, 15, 0
	}


	quantized |= discard << 45;

	if (floats[discard] < 0)
		quantized |= 1ui64 << 47;

	return quantized;
}

uint16_t pa::Quantization::quantizeFloat(float value)
{
	// NOTE
	// if the value is sqrt(2) then result is 16383
	// guarantees a numerical precision of 0.000043
	// that value has been acquired from 
	// 1 /(16383 * sqrt(2)) = 4.3161007213974700872907548196597e-5

	constexpr uint16_t	maxQuantizeValue = 16383;			// 2^14 - 1;
	constexpr float		sqrt2 = 1.41421356237309504880f;	// sqrt(2)

	int16_t	result = maxQuantizeValue * static_cast<int16_t>(sqrt2 * value);
	result += maxQuantizeValue;

	assert(result >= 0 && "result must be positive");

	return static_cast<uint16_t>(result);
}

float pa::Quantization::deQuantizeFloat(uint64_t quantized, size_t index)
{
	constexpr uint16_t	maxQuantizeValue = 16383;			// 2^14 - 1;
	constexpr float		sqrt2 = 1.41421356237309504880f;	// sqrt(2)

	int16_t extracted = static_cast<int16_t>(quantized >> (30 * (-15 * index))) & 0x7FFF;

	extracted -= maxQuantizeValue;

	float	result = static_cast<float>(extracted) / maxQuantizeValue / sqrt2;
	return result;
}

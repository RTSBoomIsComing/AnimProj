// author: Changwan Yu
// reference: https://technology.riotgames.com/news/compressing-skeletal-animation-data
// Compress quaternion
// quantize 32bit float to 15bit
// total structure size is 48bit
// This 48-bit quantization of quaternions guarantees a numerical precision of 0.000043
#pragma once

namespace pa
{
	class Quantization
	{
	public:
		Quantization(const DirectX::XMFLOAT4& quaternion);
		~Quantization() = default;

		DirectX::XMVECTOR deQuantize();
	private:
		int16_t quantizeFloat(float value) const;

	private:
		uint64_t _data : 48;
	};

}
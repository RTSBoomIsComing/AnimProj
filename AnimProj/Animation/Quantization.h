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
		static DirectX::XMVECTOR deQuantize(uint64_t quantized);
	private:
		static uint16_t		quantizeFloat(float value);
		static float		deQuantizeFloat(uint64_t quantized, size_t index);

	private:
		uint64_t _data;
	};

}
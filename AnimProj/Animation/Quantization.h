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
		static DirectX::XMFLOAT4 deQuantize(uint64_t quantized);
		static uint64_t		quantize(const DirectX::XMFLOAT4& vector4);
	private:
		static uint16_t		quantizeFloat(float value);
		static float		deQuantizeFloat(uint64_t quantized, size_t index);

	private:
		Quantization() = default;
		~Quantization() = default;
	};

}
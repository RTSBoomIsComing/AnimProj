// author: Changwan Yu
#pragma once

namespace pa
{
	class RawAnimation
	{
		friend class AcclaimImporter;
	public:
		struct Track
		{
			enum class Type : uint16_t
			{
				Scale,
				Rotation,
				Translation
			};

			std::vector<uint16_t>			times;
			std::vector<DirectX::XMFLOAT4>	values;
			
			uint16_t			boneID	: 14;
			Type				type	: 2;
		};

	public:
		RawAnimation() = default;
		~RawAnimation() = default;

	public:
		std::vector<Track> _tracks;
	};
}


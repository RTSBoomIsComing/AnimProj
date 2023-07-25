// author: Changwan Yu
#pragma once
namespace pa
{
	struct CompactKey
	{
		union
		{
			uint16_t							v[4]		= {};
			DirectX::PackedVector::XMHALF4		asHalf4;
			DirectX::PackedVector::XMUSHORT4	asUShort4;

			struct
			{
				uint16_t x;
				uint16_t y;
				uint16_t z;

				uint16_t	keytime	: 13;
				uint16_t	opt_1	: 2;
				uint16_t	opt_0	: 1;
			};
		};

		static CompactKey createFromQuaternion(DirectX::XMVECTOR Q);
		static CompactKey createFromVector(DirectX::XMVECTOR V);

		DirectX::XMVECTOR decompressAsQuaternion()	const;
		DirectX::XMVECTOR decompressAsVector()		const;
	};
}

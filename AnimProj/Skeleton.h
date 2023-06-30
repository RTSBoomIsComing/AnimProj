#pragma once
namespace pa
{
	class Skeleton
	{
	public:
		struct Bone
		{
			enum class DOF : std::uint8_t
			{
				RX, RY, RZ, LN /* length */,
				TX, TY, TZ, None
			};

			int					id = 0;
			std::string			name;

			DirectX::XMFLOAT4	axis = {};
			DirectX::XMFLOAT4	direction = {};
			float				length = 0.0f;
			char				axisOrder[4] = {}; // XYZ with \0
			DOF					dof[4] = { DOF::None, DOF::None, DOF::None, DOF::None };
		};


	};
}


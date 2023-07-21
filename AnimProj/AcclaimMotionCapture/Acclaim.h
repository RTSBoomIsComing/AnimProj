#pragma once
namespace pa
{
	class Acclaim
	{
	public:
		class Skeleton;
		class Motion;

		struct Unit
		{
			float length = 1.0f;
			float mass = 1.0f;
			float angle = 1.0f;
		};

		enum class DOF : uint16_t
		{
			None, TX, TY, TZ, RX, RY, RZ, L
		};

		enum class AxisOrder : uint16_t
		{
			XYZ, XZY, YZX, YXZ, ZXY, ZYX
		};

	public:
		static Acclaim::DOF			convertStringToDOF(std::string const& s);
		static Acclaim::AxisOrder	convertStringToAxisOrder(std::string const& s);
		static DirectX::XMVECTOR	getQuaternionFromAxis(DirectX::XMVECTOR axis, AxisOrder order);

	private:
		Acclaim() = delete;
		~Acclaim() = delete;
	};
}


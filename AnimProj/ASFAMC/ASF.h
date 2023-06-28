// author: Changwan Yu
#pragma once
namespace pa
{
	class ASF
	{
	public:
		enum class UnitAngle : std::uint8_t
		{
			Degree, Radian
		};
	public:
		ASF() = default;
		~ASF() = default;
		ASF(const wchar_t* filePath);

		bool loadASFFromFile(const wchar_t* filePath);

	private:
		float		_unitMass;
		float		_unitLength;
		UnitAngle	_unitAngle;



	};
}


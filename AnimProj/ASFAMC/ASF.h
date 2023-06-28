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
		void ParseUnits(std::ifstream& stream);
		void ParseRoot(std::ifstream& stream);
		void ParseBoneData(std::ifstream& stream);
		void ParseHierarchy(std::ifstream& stream);
	private:
		float		_unitMass;
		float		_unitLength;
		UnitAngle	_unitAngle;



	};
}


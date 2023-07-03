// author: Changwan Yu
#include "pch.h"
#include "AMC.h"
#include "ASF.h"

pa::AMC::AMC(const wchar_t* filePath)
{
	if (loadFromFile(filePath) != true)
		DebugBreak();
}

bool pa::AMC::loadFromFile(const wchar_t* filePath)
{
	std::ifstream stream{ filePath };
	if (stream.fail())
		return false;

	// boneCount will be provided by Skeleton
	// int boneCount;

	_frameCount = 0;
	char line[200] = {};
	char boneName[100] = {};
	while (stream.getline(line, sizeof(line), '\n'))
	{
		if ('#' == line[0])
			continue;

		if (':' == line[0])
			continue;

		if (std::isdigit(line[0]))
		{
			_frameCount++;
			continue;	// start motion data by frame
		}

		std::stringstream subStream(line);

		// the motion data order of each frame is same
		// so when read data of first frame, get the names of the bones
		subStream >> boneName;
		if (1 == _frameCount)
			_dataOrder.push_back(boneName);

		float buffer;
		while (subStream >> buffer)
		{
			_data.push_back(buffer);
		}
	}

	return true;
}

void pa::AMC::generateAnimation(const ASF* pASF)
{
	using namespace DirectX;
	// Match order
	std::vector<int> orderMatch;
	orderMatch.reserve(_dataOrder.size());

	for (const auto& name : _dataOrder)
	{
		auto it = std::find(pASF->_boneNameList.begin(), pASF->_boneNameList.end(), name);
		size_t index = it - pASF->_boneNameList.begin();
		orderMatch.push_back(static_cast<int>(index));
	}

	_animationSheets.resize(_frameCount);
	size_t dataIndex = 0;
	for (int frameID = 0; frameID < _frameCount; frameID++)
	{
		_animationSheets[frameID].rotations.resize(pASF->_boneData.size());
		for (const int boneIndex : orderMatch)
		{
			// Skip processing root bone
			// TODO: need to fix, root bone also should be processed in this block
			if (0 == boneIndex)
			{
				_animationSheets[frameID].rotations[0] = XMMatrixIdentity();
				dataIndex += 6;
				continue;
			}

			float channels[4] = {};
			for (int j = 0; j < 4; j++)
			{
				const Bone::DOF& dof = pASF->_boneData[boneIndex].dof[j];
				if (Bone::DOF::LN == dof)
					DebugBreak();

				if (Bone::DOF::None == dof)
					break;

				channels[static_cast<int>(dof)] = _data[dataIndex++];
			}
			// Ignore AxisOrder, just calculate rotation in order of X, Y, Z
			// TODO: Need to process AxisOrder
			XMMATRIX rotation = 
				// For converting coordiate system right handed to left handed,
				// amounts of rotation by X, Y axis would be revert.
				XMMatrixRotationX(-channels[0])* XMMatrixRotationY(-channels[1])* XMMatrixRotationZ(channels[2]);
			_animationSheets[frameID].rotations[boneIndex] = rotation;
		}
	}
}

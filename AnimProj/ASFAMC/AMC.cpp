// author: Changwan Yu
#include "pch.h"
#include "AMC.h"
#include "ASF.h"
#include "../Rendering/Animation.h"

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

void pa::AMC::generateAnimation(const ASF* pASF, Animation* pAnimation)
{
	using namespace DirectX;

	std::vector<int> orderMatch;
	orderMatch.reserve(_dataOrder.size());

	for (const auto& name : _dataOrder)
	{
		auto it = std::find(pASF->_boneNameList.begin(), pASF->_boneNameList.end(), name);
		size_t index = it - pASF->_boneNameList.begin();
		orderMatch.push_back(static_cast<int>(index));
	}

	pAnimation->initialize(_frameCount, pASF->getBoneCount());

	size_t dataIndex = 0;
	for (int frameID = 0; frameID < _frameCount; frameID++)
	{
		//_animationSheets[frameID].rotations.resize(pASF->getBoneCount(), XMMatrixIdentity());
		for (const int boneIndex : orderMatch)
		{
			// rx, ry, rz, tx, ty, tz, l
			float dataBuffer[7] = {};
			for (int j = 0; j < 7; j++)
			{
				const ASF::Channel& channel = pASF->_DOFs[boneIndex].channels[j];
				if (ASF::Channel::LN == channel)
					DebugBreak();

				if (ASF::Channel::None == channel)
					break;

				dataBuffer[static_cast<size_t>(channel)] = _data[dataIndex++]; //* pASF->_unitAngle;
			}

			dataBuffer[0] *= pASF->_unitAngle	* -1;	// rx
			dataBuffer[1] *= pASF->_unitAngle	* -1;	// ry
			dataBuffer[2] *= pASF->_unitAngle;			// rz

			dataBuffer[3] *= pASF->_unitLength;			// tx
			dataBuffer[4] *= pASF->_unitLength;			// ty
			dataBuffer[5] *= pASF->_unitLength	* -1;	// tz

			dataBuffer[6] *= pASF->_unitLength;			// l (length)

			XMMATRIX rotation = ASF::eulerRotation(dataBuffer, pASF->_axisOrders[boneIndex]);
			XMVECTOR quaternion = XMQuaternionRotationMatrix(rotation);
			XMStoreFloat4(&pAnimation->getRotation(frameID, boneIndex), quaternion);
			pAnimation->getPosition(frameID, boneIndex) = XMFLOAT4(dataBuffer[3], dataBuffer[4], dataBuffer[5], 1.0f);
		}
	}
}

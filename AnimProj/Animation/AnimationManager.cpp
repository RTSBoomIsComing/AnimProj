#include "pch.h"
#include "AnimationManager.h"
#include "Skeleton.h"
#include "../AcclaimMotionCapture/AcclaimMotion.h"
#include "../AcclaimMotionCapture/AcclaimSkeleton.h"
#include "../Animation/AcclaimImporter.h"
#include "../Animation/AnimationBuilder.h"


pa::AnimationManager::AnimationManager()
{
}

void pa::AnimationManager::initialize()
{
	std::wstring asfFilePath = _SOLUTIONDIR;

	asfFilePath += LR"(Assets\ASFAMC\integrate\default.asf)";
	Acclaim::Skeleton acclaimSkeleton(asfFilePath);

	_skeletonList.emplace_back();
	AcclaimImporter::createSkeleton(acclaimSkeleton, &_skeletonList.back());
	_skeletonList.back().generateBoneMasks();
	_skeletonList.back().generateBoneToBoneLTs();

	std::wstring amcDirectory = _SOLUTIONDIR;
	amcDirectory += LR"(Assets\ASFAMC\integrate\)";

	{
		Acclaim::Motion acclaimMotion(&acclaimSkeleton, amcDirectory + LR"(walk.amc)");
		RawAnimation rawAnimation = {};
		AcclaimImporter::createRawAnimation(acclaimMotion, this->getDefaultSkeleton(), &rawAnimation);
		AnimationBuilder animationBuilder(this->getDefaultSkeleton(), rawAnimation);

		_animationList.push_back(animationBuilder.createUpperBodyAnimation());
		_animationList.push_back(animationBuilder.createLowerBodyAnimation());
	}

	{
		Acclaim::Motion acclaimMotion(&acclaimSkeleton, amcDirectory + LR"(run.amc)");
		RawAnimation rawAnimation = {};
		AcclaimImporter::createRawAnimation(acclaimMotion, this->getDefaultSkeleton(), &rawAnimation);
		AnimationBuilder animationBuilder(this->getDefaultSkeleton(), rawAnimation);

		_animationList.push_back(animationBuilder.createUpperBodyAnimation());
		_animationList.push_back(animationBuilder.createLowerBodyAnimation());
	}

	{
		Acclaim::Motion acclaimMotion(&acclaimSkeleton, amcDirectory + LR"(punch.amc)");
		RawAnimation rawAnimation = {};
		AcclaimImporter::createRawAnimation(acclaimMotion, this->getDefaultSkeleton(), &rawAnimation);
		AnimationBuilder animationBuilder(this->getDefaultSkeleton(), rawAnimation);

		_animationList.push_back(animationBuilder.createUpperBodyAnimation());
		_animationList.push_back(animationBuilder.createLowerBodyAnimation());
	}

	{
		Acclaim::Motion acclaimMotion(&acclaimSkeleton, amcDirectory + LR"(jump.amc)");
		RawAnimation rawAnimation = {};
		AcclaimImporter::createRawAnimation(acclaimMotion, this->getDefaultSkeleton(), &rawAnimation);
		AnimationBuilder animationBuilder(this->getDefaultSkeleton(), rawAnimation);

		_animationList.push_back(animationBuilder.createUpperBodyAnimation());
		_animationList.push_back(animationBuilder.createLowerBodyAnimation());
	}

	{
		Acclaim::Motion acclaimMotion(&acclaimSkeleton, amcDirectory + LR"(dance.amc)");
		RawAnimation rawAnimation = {};
		AcclaimImporter::createRawAnimation(acclaimMotion, this->getDefaultSkeleton(), &rawAnimation);
		AnimationBuilder animationBuilder(this->getDefaultSkeleton(), rawAnimation);

		_animationList.push_back(animationBuilder.createUpperBodyAnimation());
		_animationList.push_back(animationBuilder.createLowerBodyAnimation());
	}

}

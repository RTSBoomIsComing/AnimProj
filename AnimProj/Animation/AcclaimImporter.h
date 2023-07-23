// author: Changwan Yu
#pragma once
#include "../AcclaimMotionCapture/Acclaim.h"

namespace pa
{
	class RawAnimation;
	class Skeleton;
	class AcclaimImporter
	{
	public:
		static void	createRawAnimation(Acclaim::Motion const& acclaimMotion, const Skeleton& skeleton, RawAnimation* rawAnimation);
		static void	destroyRawAnimation(RawAnimation* rawAnimation);

		static void	createSkeleton(Acclaim::Skeleton const& acclaimSkeleton, Skeleton* skeleton);
		static void	destroySkeleton(Skeleton* skeleton);
	};
}


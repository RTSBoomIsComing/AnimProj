// author: Changwan Yu
#include "pch.h"
#include "AcclaimImporter.h"

void pa::AcclaimImporter::createRawAnimation(Acclaim::Motion const& acclaimMotion, RawAnimation* rawAnimation)
{
}

void pa::AcclaimImporter::destroyRawAnimation(RawAnimation* rawAnimation)
{
	delete rawAnimation;
}

void pa::AcclaimImporter::createSkeleton(Acclaim::Skeleton const& acclaimSkeleton, Skeleton* skeleton)
{
	delete skeleton;
}

void pa::AcclaimImporter::destroySkeleton(Skeleton* skeleton)
{
	delete skeleton;
}

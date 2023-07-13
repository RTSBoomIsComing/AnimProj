#pragma once
namespace pa
{
	class Skeleton;
	class SkeletonMesh
	{
	public:
		SkeletonMesh();
		~SkeletonMesh();

	private:
		Skeleton* _skeleton;

	};
}



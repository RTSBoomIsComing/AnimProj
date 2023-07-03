#pragma once

namespace pa
{
	// If Any class inherit IRendering
	// Then the class is in charge of rendering process
	// That means the class can have D3D11 Objects
	// Or can have commands to GPU.
	class Rendering
	{
	public:
		class Camera;
		class Screen;
		class Mesh;
		class Skeleton;
		class Animation;

	private:
		Rendering()	= default;
		~Rendering()	= default;
	};
}
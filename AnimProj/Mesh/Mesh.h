// author: Changwan Yu
#pragma once

namespace pa
{
	class Mesh
	{
	public:
		Mesh() = default;
		virtual ~Mesh() = default;
		virtual void render() abstract;
	};
}


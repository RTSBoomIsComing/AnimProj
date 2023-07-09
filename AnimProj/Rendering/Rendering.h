// author: Changwan Yu
#pragma once

namespace pa
{
	class Rendering
	{
	public:
		Rendering() = default;
		virtual ~Rendering() = default;

		virtual void render(ID3D11DeviceContext* pContext) abstract;
	};
}



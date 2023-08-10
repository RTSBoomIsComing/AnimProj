// author: Changwan Yu
#pragma once

namespace pa
{
	class Actor;
	class SceneComponent
	{
	public:
		SceneComponent();
		~SceneComponent();
	public:
		DirectX::XMFLOAT3 position	= {};
		DirectX::XMFLOAT3 euler		= {};
		std::list<std::weak_ptr<Actor>>::iterator mapIterator;
	};
}


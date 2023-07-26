// author: Changwan Yu
#pragma once

namespace pa
{
	class AnimationPlayer;
	class AnimationBlender
	{
	public:
		AnimationBlender(const AnimationPlayer& base, const AnimationPlayer& target);
		~AnimationBlender();
	private:
	};

}


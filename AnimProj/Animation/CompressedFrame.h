#pragma once

namespace pa
{
	enum class ChannelType : uint16_t
	{
		Scale, Rotation, Translation, Direction
	};

	class CompressedFrame
	{
	public:
		CompressedFrame();
		~CompressedFrame() = default;

		void			setChannelType(ChannelType type);
		ChannelType		getChannelType()					const;

		void			setJointIndex(uint16_t jointIndex);
		uint16_t		getJointIndex()						const;

		void			setKeyTime(uint16_t keyTime);
		uint16_t		getKeyTime()						const;

		void			setQuantized(uint64_t quantized);
		uint64_t		getQuantized()						const;

	private:
		// The time interval is 1/120 second.
		uint16_t _keyTime		= 0;

		// Most significant two bits of this 8bit index contains channel type info.
		uint16_t _jointIndex	= 0;		

		// Payload, which can be a quantized 3d vector or a quantized quaternion
		// depending on what type of channel this data belongs to
		uint16_t _v[3]			= {};
	};
}



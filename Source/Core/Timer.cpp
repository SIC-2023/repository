#include "Timer.h"

#include "../Utility/Misc.h"

namespace argent
{
	Timer::Timer()
	{
		LONGLONG counts_per_second;
		//��b�Ԃɉ���J�E���g�ł��邩
		QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_second));

		//1�J�E���g�ɉ��b�����邩
		seconds_per_count = 1.0 / static_cast<double>(counts_per_second);

		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time_));
		base_time_ = this_time_;
		last_time_ = this_time_;
		delta_time_ = 0.0f;
	}

	void Timer::OnAwake()
	{
		delta_time_ = 0;
		Subsystem::OnAwake();
	}

	void Timer::OnShutdown()
	{
		//TODO �^�C�}�[�̃V���b�g�_�E���ł�邱�Ƃ͂���H
		Subsystem::OnShutdown();
	}

	void Timer::Tick()
	{
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&this_time_));
		delta_time_ = static_cast<float>(static_cast<double>(this_time_ - last_time_) * seconds_per_count);
		last_time_ = this_time_;
		if(delta_time_ < 0.0f)
		{
			delta_time_ = 0.0f;
		}
		CalcFrameTime();
	}

	void Timer::CalcFrameTime()
	{
		const auto stamp = static_cast<float>((static_cast<double>(this_time_ - base_time_) * seconds_per_count));
		++frames_;
		if (stamp - elapsed_time_ >= 1.0f)
		{
			const auto fps = static_cast<float>(frames_);
			frame_times_ = 1000.0f / fps;
			elapsed_time_ += 1.0f;
			frames_per_second_ = frames_;
			frames_ = 0;
		}
	}
}

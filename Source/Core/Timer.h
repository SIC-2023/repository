#pragma once
#include <Windows.h>

#include "../SubSystem/Subsystem.h"

namespace argent
{
	class Timer final:
		public Subsystem
	{
	public:

		Timer();

		virtual ~Timer() = default;

	public:

		void OnAwake() override;
		void OnShutdown() override;

		float GetDeltaTime() const { return delta_time_;}
		float GetFrameTimes() const { return frame_times_; }
		UINT GetFps() const { return frames_per_second_; }
		void Tick();

	private:
		void CalcFrameTime();

	private:
		float elapsed_time_{};
		double seconds_per_count{};
		UINT frames_{};

		LONGLONG this_time_{};
		LONGLONG last_time_{};
		LONGLONG base_time_{};

		float delta_time_{};
		UINT frames_per_second_{};
		float frame_times_{};
	};
}


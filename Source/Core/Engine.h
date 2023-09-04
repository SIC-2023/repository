#pragma once

#include "../SubSystem/Subsystem.h"

namespace argent
{
	class Engine;

	Engine* GetEngine();

	class Engine
	{
	public:
		Engine();
		~Engine();

		void SetUp();
		void OnShutdown();

		void Run();

		void RequestShutdown() { is_request_showdown_ = true; }
		bool GetIsRequestShutdown() const { return is_request_showdown_; }

		SubsystemLocator& GetSubsystemLocator()
		{
			return subsystem_locator_;
		}
	private:
		SubsystemLocator subsystem_locator_;
		bool is_request_showdown_;
	};
}


#pragma once

#include "../SubSystem/Subsystem.h"

namespace argent
{
	class Engine;

	Engine* GetEngine();

	/**
	 * \brief Managing all subsystem and run(update and rendering) the application
	 * ban this class from instantiating multiply
	 */
	class Engine
	{
	public:
		Engine();
		~Engine();

		Engine(const Engine&) = delete;
		Engine(const Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(const Engine&&) = delete;

		/**
		 * \brief set up engine and various subsystem
		 */
		void SetUp();

		/**
		 * \brief shutdown various subsystem and engine
		 */
		void OnShutdown();

		/**
		 * \brief update varaious subsystem and rendering scene
		 */
		void Run();

		/**
		 * \brief to call if you want to shutdown the application
		 */
		void RequestShutdown() { is_request_showdown_ = true; }
		bool GetIsRequestShutdown() const { return is_request_showdown_; }

		SubsystemLocator& GetSubsystemLocator()
		{
			return subsystem_locator_;
		}

	private:
		SubsystemLocator subsystem_locator_;	//contain all subsystem
		bool is_request_showdown_;
		bool is_editor_mode_ = false;
	};
}


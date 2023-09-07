#pragma once

#include "../SubSystem/Subsystem.h"

namespace argent::rendering
{
	class RenderingManager final: public Subsystem
	{
	public:
		RenderingManager();
		~RenderingManager() override;

		void OnAwake() override;
		void OnShutdown() override;


	};
}


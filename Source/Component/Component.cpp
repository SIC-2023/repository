#include "Component.h"

namespace argent::component
{
	void Component::SetIsActive(bool active_flag)
	{
		if(!is_active_ && active_flag)
		{
			OnEnable();
		}
		is_active_ = active_flag;
	}
}

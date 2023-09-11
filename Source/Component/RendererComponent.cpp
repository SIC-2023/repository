#include "RendererComponent.h"

#include "../Core/Engine.h"
#include "../Rendering/RenderingManager.h"

namespace argent::component
{
	RendererComponent::RendererComponent(std::string name)
	: Component(name)
	{
		GetEngine()->GetSubsystemLocator().Get<rendering::RenderingManager>()->Register(this);
	}
}

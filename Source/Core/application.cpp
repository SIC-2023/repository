#include "application.h"

#include <d3d12.h>

#include "../Utility/Misc.h"

namespace argent
{
#ifdef _DEBUG
	void EnableDebugLayer()
	{
		HRESULT hr{ S_OK };
		ID3D12Debug* debugLayer = nullptr;
		hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer));
		_ARGENT_ASSERT_EXPR(hr);

		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
#endif

	void Application::SetUp(bool is_editor_mode)
	{
#ifdef _DEBUG 
	EnableDebugLayer();

	//ƒƒ‚ƒŠƒŠ[ƒN‚ÌŒŸo
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc();
#endif

		engine_ = std::make_unique<Engine>(is_editor_mode);
		engine_->SetUp();
	}

	int Application::OnShutdown()
	{
		engine_->OnShutdown();
		return 0;
	}

	void Application::Run()
	{
		while (!engine_->GetIsRequestShutdown())
		{
			engine_->Run();
		}
	}
}

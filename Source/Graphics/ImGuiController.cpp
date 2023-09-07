#include "ImGuiController.h"

#include <DirectXMath.h>
#include <windowsx.h>

#include "imgui.h"

#include "../Core/Timer.h"
#include "../Utility/Misc.h"
#include "../../Assets/Shader/ImGui.hlsli"
#include "../Core/Engine.h"


#define IM_VK_KEYPAD_ENTER (VK_RETURN + 256)
enum class MONITOR_DPI_TYPE { MDT_EFFECTIVE_DPI = 0, MDT_ANGULAR_DPI = 1, MDT_RAW_DPI = 2, MDT_DEFAULT = MDT_EFFECTIVE_DPI } ;
typedef HRESULT(WINAPI* PFN_GetDpiForMonitor)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);        // Shcore.lib + dll, Windows 8.1+


namespace argent::graphics
{
	//ImGuiのstaticメンバ
	HWND ImGuiController::hwnd_ = nullptr;
	int ImGuiController::mouse_tracked_area_ = 0;
	int ImGuiController::mouse_buttons_down_ = 0;
	bool ImGuiController::want_update_monitors_ = false;

	//色々ヘルプ関数
	ImGuiMouseSource GetMouseSourceFromMessageExtraInfo();
	bool UpdateMouseCursor();
	void AddKeyEvent(ImGuiKey key, bool down, int native_keycode, int native_scancode = -1);
	ImGuiKey VirtualKeyToImGuiKey(WPARAM wParam);
	BOOL IsWindowsVersionOrGreater(WORD major, WORD minor, WORD);
	float GetDpiScaleForMonitor(void* monitor);
	BOOL CALLBACK UpdateMonitors_EnumFunc(HMONITOR monitor, HDC, LPRECT, LPARAM);

	static bool IsVkDown(int vk)
	{
	    return (::GetKeyState(vk) & 0x8000) != 0;
	}


	static void ImGui_ImplWin32_ProcessKeyEventsWorkarounds()
	{
	    // Left & right Shift keys: when both are pressed together, Windows tend to not generate the WM_KEYUP event for the first released one.
	    if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && !IsVkDown(VK_LSHIFT))
	       AddKeyEvent(ImGuiKey_LeftShift, false, VK_LSHIFT);
	    if (ImGui::IsKeyDown(ImGuiKey_RightShift) && !IsVkDown(VK_RSHIFT))
	        AddKeyEvent(ImGuiKey_RightShift, false, VK_RSHIFT);

	    // Sometimes WM_KEYUP for Win key is not passed down to the app (e.g. for Win+V on some setups, according to GLFW).
	    if (ImGui::IsKeyDown(ImGuiKey_LeftSuper) && !IsVkDown(VK_LWIN))
	        AddKeyEvent(ImGuiKey_LeftSuper, false, VK_LWIN);
	    if (ImGui::IsKeyDown(ImGuiKey_RightSuper) && !IsVkDown(VK_RWIN))
	        AddKeyEvent(ImGuiKey_RightSuper, false, VK_RWIN);
	}


	LRESULT ImGuiController::ImGuiWndProcHandler(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param)
	{
		if (ImGui::GetCurrentContext() == nullptr)
	        return 0;

	    ImGuiIO& io = ImGui::GetIO();

	    switch (msg)
	    {
	    case WM_MOUSEMOVE:
	    case WM_NCMOUSEMOVE:
	    {
	        // We need to call TrackMouseEvent in order to receive WM_MOUSELEAVE events
	        ImGuiMouseSource mouse_source = GetMouseSourceFromMessageExtraInfo();
	        const int area = (msg == WM_MOUSEMOVE) ? 1 : 2;
	        hwnd_ = hwnd;
	        if (mouse_tracked_area_ != area)
	        {
	            TRACKMOUSEEVENT tme_cancel = { sizeof(tme_cancel), TME_CANCEL, hwnd, 0 };
	            TRACKMOUSEEVENT tme_track = { sizeof(tme_track), (DWORD)((area == 2) ? (TME_LEAVE | TME_NONCLIENT) : TME_LEAVE), hwnd, 0 };
	            if (mouse_tracked_area_ != 0)
	                ::TrackMouseEvent(&tme_cancel);
	            ::TrackMouseEvent(&tme_track);
	            mouse_tracked_area_ = area;
	        }
	        POINT mouse_pos = { (LONG)GET_X_LPARAM(l_param), (LONG)GET_Y_LPARAM(l_param) };
	        bool want_absolute_pos = (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0;
	        if (msg == WM_MOUSEMOVE && want_absolute_pos)    // WM_MOUSEMOVE are client-relative coordinates.
	            ::ClientToScreen(hwnd, &mouse_pos);
	        if (msg == WM_NCMOUSEMOVE && !want_absolute_pos) // WM_NCMOUSEMOVE are absolute coordinates.
	            ::ScreenToClient(hwnd, &mouse_pos);
	        io.AddMouseSourceEvent(mouse_source);
	        io.AddMousePosEvent((float)mouse_pos.x, (float)mouse_pos.y);
	        break;
	    }
	    case WM_MOUSELEAVE:
	    case WM_NCMOUSELEAVE:
	    {
	        const int area = (msg == WM_MOUSELEAVE) ? 1 : 2;
	        if (mouse_tracked_area_ == area)
	        {
	            if (hwnd_ == hwnd)
	                hwnd_ = nullptr;
	            mouse_tracked_area_ = 0;
	            io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
	        }
	        break;
	    }
	    case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
	    case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
	    case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
	    case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	    {
	        ImGuiMouseSource mouse_source = GetMouseSourceFromMessageExtraInfo();
	        int button = 0;
	        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) { button = 0; }
	        if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) { button = 1; }
	        if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) { button = 2; }
	        if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) { button = (GET_XBUTTON_WPARAM(w_param) == XBUTTON1) ? 3 : 4; }
	        if (mouse_buttons_down_ == 0 && ::GetCapture() == nullptr)
	            ::SetCapture(hwnd);
	        mouse_buttons_down_ |= 1 << button;
	        io.AddMouseSourceEvent(mouse_source);
	        io.AddMouseButtonEvent(button, true);
	        return 0;
	    }
	    case WM_LBUTTONUP:
	    case WM_RBUTTONUP:
	    case WM_MBUTTONUP:
	    case WM_XBUTTONUP:
	    {
	        ImGuiMouseSource mouse_source = GetMouseSourceFromMessageExtraInfo();
	        int button = 0;
	        if (msg == WM_LBUTTONUP) { button = 0; }
	        if (msg == WM_RBUTTONUP) { button = 1; }
	        if (msg == WM_MBUTTONUP) { button = 2; }
	        if (msg == WM_XBUTTONUP) { button = (GET_XBUTTON_WPARAM(w_param) == XBUTTON1) ? 3 : 4; }
	        mouse_buttons_down_ &= ~(1 << button);
	        if (mouse_buttons_down_ == 0 && ::GetCapture() == hwnd)
	            ::ReleaseCapture();
	        io.AddMouseSourceEvent(mouse_source);
	        io.AddMouseButtonEvent(button, false);
	        return 0;
	    }
	    case WM_MOUSEWHEEL:
	        io.AddMouseWheelEvent(0.0f, (float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA);
	        return 0;
	    case WM_MOUSEHWHEEL:
	        io.AddMouseWheelEvent(-(float)GET_WHEEL_DELTA_WPARAM(w_param) / (float)WHEEL_DELTA, 0.0f);
	        return 0;
	    case WM_KEYDOWN:
	    case WM_KEYUP:
	    case WM_SYSKEYDOWN:
	    case WM_SYSKEYUP:
	    {
	        const bool is_key_down = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
	        if (w_param < 256)
	        {
	            // Submit modifiers
	            {
				    io.AddKeyEvent(ImGuiMod_Ctrl, (GetKeyState(VK_CONTROL) & 0x8000) != 0);
				    io.AddKeyEvent(ImGuiMod_Shift, (GetKeyState(VK_SHIFT) & 0x8000) != 0);
				    io.AddKeyEvent(ImGuiMod_Alt, (GetKeyState(VK_MENU) & 0x8000) != 0);
				    io.AddKeyEvent(ImGuiMod_Super, (GetKeyState(VK_APPS) & 0x8000) != 0);
				}

	            // Obtain virtual key code
	            // (keypad enter doesn't have its own... VK_RETURN with KF_EXTENDED flag means keypad enter, see IM_VK_KEYPAD_ENTER definition for details, it is mapped to ImGuiKey_KeyPadEnter.)
	            int vk = (int)w_param;
	            if ((w_param == VK_RETURN) && (HIWORD(l_param) & KF_EXTENDED))
	                vk = IM_VK_KEYPAD_ENTER;

	            // Submit key event
	            const ImGuiKey key = VirtualKeyToImGuiKey(vk);
	            const int scancode = (int)LOBYTE(HIWORD(l_param));
	            if (key != ImGuiKey_None)
	                AddKeyEvent(key, is_key_down, vk, scancode);

				const int is_key_down_int = is_key_down ? 1 : 0;
	            // Submit individual left/right modifier events
	            if (vk == VK_SHIFT)
	            {
	                // Important: Shift keys tend to get stuck when pressed together, missing key-up events are corrected in ImGui_ImplWin32_ProcessKeyEventsWorkarounds()
	                if ((GetKeyState(VK_LSHIFT) & 0x8000) == is_key_down_int) { AddKeyEvent(ImGuiKey_LeftShift, is_key_down_int, VK_LSHIFT, scancode); }
	                if ((GetKeyState(VK_RSHIFT) & 0x8000) == is_key_down_int) { AddKeyEvent(ImGuiKey_RightShift, is_key_down_int, VK_RSHIFT, scancode); }
	            }
	            else if (vk == VK_CONTROL)
	            {
	                if ((GetKeyState(VK_LCONTROL) & 0x8000) == is_key_down_int) { AddKeyEvent(ImGuiKey_LeftCtrl, is_key_down_int, VK_LCONTROL, scancode); }
	                if ((GetKeyState(VK_RCONTROL) & 0x8000) == is_key_down_int) { AddKeyEvent(ImGuiKey_RightCtrl, is_key_down_int, VK_RCONTROL, scancode); }
	            }
	            else if (vk == VK_MENU)
	            {
	                if ((GetKeyState(VK_LMENU) & 0x8000) == is_key_down_int) { AddKeyEvent(ImGuiKey_LeftAlt, is_key_down_int, VK_LMENU, scancode); }
	                if ((GetKeyState(VK_RMENU) & 0x8000) == is_key_down_int) { AddKeyEvent(ImGuiKey_RightAlt, is_key_down_int, VK_RMENU, scancode); }
	            }
	        }
	        return 0;
	    }
	    case WM_SETFOCUS:
	    case WM_KILLFOCUS:
	        io.AddFocusEvent(msg == WM_SETFOCUS);
	        return 0;
	    case WM_CHAR:
	        if (::IsWindowUnicode(hwnd))
	        {
	            // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
	            if (w_param > 0 && w_param < 0x10000)
	                io.AddInputCharacterUTF16((unsigned short)w_param);
	        }
	        else
	        {
	            wchar_t wch = 0;
	            ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*)&w_param, 1, &wch, 1);
	            io.AddInputCharacter(wch);
	        }
	        return 0;
	    case WM_SETCURSOR:
	        // This is required to restore cursor when transitioning from e.g resize borders to client area.
	        if (LOWORD(l_param) == HTCLIENT && UpdateMouseCursor())
	            return 1;
	        return 0;
	    case WM_DISPLAYCHANGE:
	        want_update_monitors_ = true;
	        return 0;
	    }
	    return 0;
	}

	//imguiの初期化処理
	void ImGuiController::Initialize(HWND hwnd, ID3D12Device* device, const dx12::Descriptor& srv_descriptor,
		UINT descriptor_increment_size, size_t descriptor_heap_pointer_start)
	{
		hwnd_ = hwnd;
        descriptor_increment_size_ = descriptor_increment_size;
        descriptor_heap_gpu_pointer_start_ = descriptor_heap_pointer_start;

		IMGUI_CHECKVERSION();
		if (!ImGui::CreateContext()) _ASSERT_EXPR(FALSE, L"ImGuiの初期化に失敗しました");

		//フォントの追加
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("./Assets/Fonts/HGRME.TTC", 16.0f, nullptr);

		//ドッキング可能に
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		//-------------------------------------Win32 Initialize-------------------------------------//

		// Setup backend capabilities flags
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
        io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport; // We can call io.AddMouseViewportEvent() with correct data (optional)

        // Our mouse update function expect PlatformHandle to be filled for the main viewport
        ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		main_viewport->PlatformHandle = main_viewport->PlatformHandleRaw = static_cast<void*>(hwnd);

		//-------------------------------------Dx12 Initialize-------------------------------------//

		//バックエンドの設定
		//何に使われているのかよくわからない
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

        font_srv_descriptor_ = srv_descriptor;
        CreateFontTexture(device);

		//-------------------------------------ImGuiとは別-------------------------------------//

		//描画パイプラインの作成
        static D3D12_INPUT_ELEMENT_DESC local_layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)IM_OFFSETOF(ImDrawVert, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (UINT)IM_OFFSETOF(ImDrawVert, uv),  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (UINT)IM_OFFSETOF(ImDrawVert, col), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        };

		D3D12_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        desc.StencilEnable = false;
        desc.FrontFace.StencilFailOp = desc.FrontFace.StencilDepthFailOp = desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        desc.BackFace = desc.FrontFace;

        {
	        D3D12_GRAPHICS_PIPELINE_STATE_DESC graphics_pipeline_state_desc{};
            graphics_pipeline_state_desc.NodeMask = 1u;
            graphics_pipeline_state_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            graphics_pipeline_state_desc.SampleMask = UINT_MAX;
            graphics_pipeline_state_desc.NumRenderTargets = 1;
            graphics_pipeline_state_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
            graphics_pipeline_state_desc.SampleDesc.Count = 1;
            graphics_pipeline_state_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
            graphics_pipeline_state_desc.InputLayout = { local_layout, 3 };
            graphics_pipeline_state_desc.BlendState = dx12::GetBlendDesc(dx12::BlendMode::Alpha);
            graphics_pipeline_state_desc.RasterizerState = dx12::GetRasterizerDesc(dx12::RasterizerMode::CullNoneSolid, false);
            graphics_pipeline_state_desc.DepthStencilState = desc;

            graphics_pipeline_state_ = std::make_unique<GraphicsPipelineState>(device, graphics_pipeline_state_desc, 
L"./Assets/Shader/ImGuiVS.hlsl", L"./Assets/Shader/ImGuiPS.hlsl");
			want_update_monitors_ = true;
        }

		HRESULT hr;
		{
		    D3D12_HEAP_PROPERTIES heap_props{};
			heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
			heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		    D3D12_RESOURCE_DESC vertex_resource_desc{};
		    vertex_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		    vertex_resource_desc.Width = MaxNumVertex * sizeof(ImDrawVert);
		    vertex_resource_desc.Height = 1;
		    vertex_resource_desc.DepthOrArraySize = 1;
		    vertex_resource_desc.MipLevels = 1;
		    vertex_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		    vertex_resource_desc.SampleDesc.Count = 1;
		    vertex_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		    vertex_resource_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		    D3D12_RESOURCE_DESC index_resource_desc = vertex_resource_desc;
			index_resource_desc.Width = MaxNumIndex * sizeof(ImDrawIdx);

			for(int i = 0; i < 3; ++i)
			{
		        hr = device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE,
					&vertex_resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 
					IID_PPV_ARGS(vertex_buffer_[i].ReleaseAndGetAddressOf()));
                _ARGENT_ASSERT_EXPR(hr);

		        hr = device->CreateCommittedResource(&heap_props, D3D12_HEAP_FLAG_NONE, 
					&index_resource_desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
					IID_PPV_ARGS(index_buffer_[i].ReleaseAndGetAddressOf()));
                _ARGENT_ASSERT_EXPR(hr);
			}
		}
	}

	void ImGuiController::Finalize()
	{
		//-------------------------------------Dx12 Finalize-------------------------------------//
		ImGui::DestroyPlatformWindows();

		//-------------------------------------Win32 Finalize-------------------------------------//
		ImGui::DestroyPlatformWindows();

		//-------------------------------------ImGui Finalize-------------------------------------//
		ImGui::DestroyContext();
	}

	void ImGuiController::Begin(float window_width, float window_height)
	{
        ImGuiIO& io = ImGui::GetIO();

        // Setup display size (every frame to accommodate for window resizing)
        io.DisplaySize = ImVec2(window_width, window_height);

        //Update Monitors
		if(want_update_monitors_)
		{
			ImGui::GetPlatformIO().Monitors.resize(0);
		    ::EnumDisplayMonitors(nullptr, nullptr, UpdateMonitors_EnumFunc, 0);
		    want_update_monitors_ = false;
		}

        // Setup time step
        INT64 current_time = 0;
        ::QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
        //io.DeltaTime = GetTimer()->GetDeltaTime();
		io.DeltaTime = GetEngine()->GetSubsystemLocator().Get<Timer>()->GetDeltaTime();

		UpdateImGuiMouseData();

        //todo ImGui Implから消した
		ImGui_ImplWin32_ProcessKeyEventsWorkarounds();

        // Update OS mouse cursor with the cursor requested by imgui
        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            ::SetCursor(nullptr);
        }
        else
        {
            // Show OS mouse cursor
            LPTSTR win32_cursor = IDC_ARROW;
            switch (imgui_cursor)
            {
            case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
            case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
            case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
            case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
            case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
            case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
            case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
            case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
            case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
            }
            ::SetCursor(::LoadCursor(nullptr, win32_cursor));
        }
		ImGui::NewFrame();
	}

	void ImGuiController::End(ID3D12GraphicsCommandList* command_list
        , UINT current_back_buffer_index)
	{
		auto timer = GetEngine()->GetSubsystemLocator().Get<Timer>();
		float delta_time = timer->GetDeltaTime();
		float frame_times = timer->GetFrameTimes();
		int fps = static_cast<int>(timer->GetFps());
		static float step{};
		static float step_fast{};
		ImGui::DragFloat("Step", &step, 0.1f, -100.0f, 100.0f);
		ImGui::DragFloat("Step Fast", &step_fast, 0.1f, -100.0f, 100.0f);
		ImGui::InputFloat("Delta Time", &delta_time, step, step_fast, "%.6f");
		ImGui::InputFloat("Frame Times", &frame_times);
		ImGui::InputInt("FPS", &fps, 0, 0);


		ImGui::Render();


        //ImGui Impl Win32
        {
            ImDrawData* draw_data = ImGui::GetDrawData();

            // Avoid rendering when minimized
		    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f)
		        return;

            HRESULT hr{ S_OK };
		    //頂点とインデックスバッファ系    絶対毎フレーム作り直すのは無駄だけど
            //一旦毎フレーム作り直す方向でいきます
		   

		    // Upload vertex/index data into a single contiguous GPU buffer
		    void* vtx_resource, *idx_resource;

		    //D3D12_RANGE range;
		    hr = vertex_buffer_[current_back_buffer_index]->Map(0, nullptr, &vtx_resource);
            _ARGENT_ASSERT_EXPR(hr);
		        
		    hr = index_buffer_[current_back_buffer_index]->Map(0, nullptr, &idx_resource);
            _ARGENT_ASSERT_EXPR(hr);


		    ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource;
		    ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource;
		    for (int n = 0; n < draw_data->CmdListsCount; n++)
		    {
		        const ImDrawList* cmd_list = draw_data->CmdLists[n];
		        memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		        vtx_dst += cmd_list->VtxBuffer.Size;
		        idx_dst += cmd_list->IdxBuffer.Size;
		    }
		    vertex_buffer_[current_back_buffer_index]->Unmap(0, nullptr);
		    index_buffer_[current_back_buffer_index]->Unmap(0, nullptr);

		    // Setup desired DX state
            RenderResource render_resource{};
            {

			    // Setup orthographic projection matrix into our constant buffer
			    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
			    {
			        float L = draw_data->DisplayPos.x;
			        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
			        float T = draw_data->DisplayPos.y;
			        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
			        float mvp[4][4] =
			        {
			            { 2.0f/(R-L),   0.0f,           0.0f,       0.0f },
			            { 0.0f,         2.0f/(T-B),     0.0f,       0.0f },
			            { 0.0f,         0.0f,           0.5f,       0.0f },
			            { (R+L)/(L-R),  (T+B)/(B-T),    0.5f,       1.0f },
			        };
			        memcpy(&render_resource.view_projection_matrix_, mvp, sizeof(mvp));
			    }

			    // Setup viewport
			    D3D12_VIEWPORT vp;
			    memset(&vp, 0, sizeof(D3D12_VIEWPORT));
			    vp.Width = draw_data->DisplaySize.x;
			    vp.Height = draw_data->DisplaySize.y;
			    vp.MinDepth = 0.0f;
			    vp.MaxDepth = 1.0f;
			    vp.TopLeftX = vp.TopLeftY = 0.0f;
			    command_list->RSSetViewports(1, &vp);

			    // Bind shader and vertex buffers
			    unsigned int stride = sizeof(ImDrawVert);
			    unsigned int offset = 0;
			    D3D12_VERTEX_BUFFER_VIEW vbv;
			    memset(&vbv, 0, sizeof(D3D12_VERTEX_BUFFER_VIEW));
			    vbv.BufferLocation = vertex_buffer_[current_back_buffer_index]->GetGPUVirtualAddress() + offset;
			    vbv.SizeInBytes = MaxNumVertex * stride;
			    //vbv.SizeInBytes = vertex_buffer_size_ * stride;
			    vbv.StrideInBytes = stride;
			    command_list->IASetVertexBuffers(0, 1, &vbv);
			    D3D12_INDEX_BUFFER_VIEW ibv;
			    memset(&ibv, 0, sizeof(D3D12_INDEX_BUFFER_VIEW));
			    ibv.BufferLocation = index_buffer_[current_back_buffer_index]->GetGPUVirtualAddress();
			    ibv.SizeInBytes = MaxNumIndex * sizeof(ImDrawIdx);
			    //ibv.SizeInBytes = index_buffer_size_ * sizeof(ImDrawIdx);
			    ibv.Format = sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
			    command_list->IASetIndexBuffer(&ibv);
			    command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			    graphics_pipeline_state_->SetOnCommandList(command_list);

            	//// Setup blend factor
			    const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
			    command_list->OMSetBlendFactor(blend_factor);
            }

		    // Render command lists
		    // (Because we merged all buffers into a single one, we maintain our own offset into them)
		    int global_vtx_offset = 0;
		    int global_idx_offset = 0;
		    ImVec2 clip_off = draw_data->DisplayPos;
		    for (int n = 0; n < draw_data->CmdListsCount; n++)
		    {
		        const ImDrawList* cmd_list = draw_data->CmdLists[n];
		        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		        {
		            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

                    //ImGui Impl から消した部分あります
		            {
		                // Project scissor/clipping rectangles into framebuffer space
		                ImVec2 clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
		                ImVec2 clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
		                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
		                    continue;

		                // Apply Scissor/clipping rectangle, Bind texture, Draw
		                const D3D12_RECT r = { (LONG)clip_min.x, (LONG)clip_min.y, (LONG)clip_max.x, (LONG)clip_max.y };

						const uint64_t tex_id = reinterpret_cast<uint64_t>(pcmd->GetTexID());
                        render_resource.texture_index_ = static_cast<uint32_t>(tex_id);

		            	command_list->SetGraphicsRoot32BitConstants(0u, sizeof(render_resource) / 4, &render_resource, 0u);
		            	command_list->RSSetScissorRects(1, &r);
		                command_list->DrawIndexedInstanced(pcmd->ElemCount, 1, pcmd->IdxOffset + global_idx_offset, pcmd->VtxOffset + global_vtx_offset, 0);
		            }
		        }
		        global_idx_offset += cmd_list->IdxBuffer.Size;
		        global_vtx_offset += cmd_list->VtxBuffer.Size;
		    }
        }
	}

	ImGuiController::ImGuiController()
	{
	}

	ImGuiController::~ImGuiController()
	{
	}

	void ImGuiController::UpdateImGuiMouseData()
	{
		ImGuiIO& io = ImGui::GetIO();

	    POINT mouse_screen_pos;
	    bool has_mouse_screen_pos = ::GetCursorPos(&mouse_screen_pos) != 0;

	    HWND focused_window = ::GetForegroundWindow();
	    const bool is_app_focused = (focused_window && (focused_window == hwnd_ || ::IsChild(focused_window, hwnd_) || ImGui::FindViewportByPlatformHandle((void*)focused_window)));
	    if (is_app_focused)
	    {
	        if (!io.WantSetMousePos && mouse_tracked_area_ == 0 && has_mouse_screen_pos)
	        {
	            POINT mouse_pos = mouse_screen_pos;
	            if (!(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable))
	                ::ScreenToClient(hwnd_, &mouse_pos);
	            io.AddMousePosEvent((float)mouse_pos.x, (float)mouse_pos.y);
	        }
	    }

		////マルチビューポートとかになるとややこしいらしい
	    ImGuiID mouse_viewport_id = 0;
	    if (has_mouse_screen_pos)
	        if (HWND hovered_hwnd = ::WindowFromPoint(mouse_screen_pos))
	            if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle((void*)hovered_hwnd))
	                mouse_viewport_id = viewport->ID;
	    io.AddMouseViewportEvent(mouse_viewport_id);
	}

	HRESULT ImGuiController::CreateFontTexture(ID3D12Device* device)
	{
        // Build texture atlas
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

        HRESULT hr{ S_OK };
        // Upload texture to graphics system
        {
            D3D12_HEAP_PROPERTIES props;
            memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
            props.Type = D3D12_HEAP_TYPE_DEFAULT;
            props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

            D3D12_RESOURCE_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            desc.Alignment = 0;
            desc.Width = width;
            desc.Height = height;
            desc.DepthOrArraySize = 1;
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            desc.Flags = D3D12_RESOURCE_FLAG_NONE;


            hr = device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
                D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(font_texture_resource_.ReleaseAndGetAddressOf()));
            IM_ASSERT(SUCCEEDED(hr));

            UINT uploadPitch = (width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
            UINT uploadSize = height * uploadPitch;
            desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            desc.Alignment = 0;
            desc.Width = uploadSize;
            desc.Height = 1;
            desc.DepthOrArraySize = 1;
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            desc.Flags = D3D12_RESOURCE_FLAG_NONE;

            props.Type = D3D12_HEAP_TYPE_UPLOAD;
            props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
            props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

            Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer = nullptr;
            hr = device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
                D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadBuffer.ReleaseAndGetAddressOf()));
            IM_ASSERT(SUCCEEDED(hr));

            void* mapped = nullptr;
            D3D12_RANGE range = { 0, uploadSize };
            hr = uploadBuffer->Map(0, &range, &mapped);
            IM_ASSERT(SUCCEEDED(hr));
            for (int y = 0; y < height; y++)
                memcpy((void*)((uintptr_t)mapped + y * uploadPitch), pixels + y * width * 4, width * 4);
            uploadBuffer->Unmap(0, &range);

            D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
            srcLocation.pResource = uploadBuffer.Get();
            srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srcLocation.PlacedFootprint.Footprint.Width = width;
            srcLocation.PlacedFootprint.Footprint.Height = height;
            srcLocation.PlacedFootprint.Footprint.Depth = 1;
            srcLocation.PlacedFootprint.Footprint.RowPitch = uploadPitch;

            D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
            dstLocation.pResource = font_texture_resource_.Get();
            dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dstLocation.SubresourceIndex = 0;

            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
            barrier.Transition.pResource = font_texture_resource_.Get();
            barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;


            Microsoft::WRL::ComPtr<ID3D12Fence> fence;
            hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.ReleaseAndGetAddressOf()));
            IM_ASSERT(SUCCEEDED(hr));

            HANDLE event = CreateEvent(0, 0, 0, 0);
            IM_ASSERT(event != nullptr);

            D3D12_COMMAND_QUEUE_DESC queueDesc = {};
            queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.NodeMask = 1;



            Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;
            hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(cmdQueue.ReleaseAndGetAddressOf()));
            IM_ASSERT(SUCCEEDED(hr));

            Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAlloc = nullptr;
            hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAlloc.ReleaseAndGetAddressOf()));
            IM_ASSERT(SUCCEEDED(hr));

            Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
            hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc.Get(), nullptr, IID_PPV_ARGS(cmdList.ReleaseAndGetAddressOf()));
            IM_ASSERT(SUCCEEDED(hr));

            cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
            cmdList->ResourceBarrier(1, &barrier);

            hr = cmdList->Close();
            IM_ASSERT(SUCCEEDED(hr));

            ID3D12CommandList* exe_command_lists[]{ cmdList.Get() };
            cmdQueue->ExecuteCommandLists(1, exe_command_lists);
            hr = cmdQueue->Signal(fence.Get(), 1);
            IM_ASSERT(SUCCEEDED(hr));

            fence->SetEventOnCompletion(1, event);
            WaitForSingleObject(event, INFINITE);

            CloseHandle(event);

        	// Create texture view
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
            ZeroMemory(&srvDesc, sizeof(srvDesc));
            srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = desc.MipLevels;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            device->CreateShaderResourceView(font_texture_resource_.Get(), &srvDesc, font_srv_descriptor_.GetCpuHandle());
        }

        //落ちた場合の対処法がImpl に書いてた
        static_assert(sizeof(ImTextureID) >= sizeof(font_srv_descriptor_.GetHeapIndex()), "Can't pack descriptor handle into TexID, 32-bit not supported yet.");
		io.Fonts->SetTexID(reinterpret_cast<ImTextureID>(font_srv_descriptor_.GetHeapIndex()));  // NOLINT(performance-no-int-to-ptr)

        return hr;
	}

	ImGuiMouseSource GetMouseSourceFromMessageExtraInfo()
	{
	    LPARAM extra_info = ::GetMessageExtraInfo();
	    if ((extra_info & 0xFFFFFF80) == 0xFF515700)
	        return ImGuiMouseSource_Pen;
	    if ((extra_info & 0xFFFFFF80) == 0xFF515780)
	        return ImGuiMouseSource_TouchScreen;
	    return ImGuiMouseSource_Mouse;
	}

	bool UpdateMouseCursor()
	{
	    ImGuiIO& io = ImGui::GetIO();
	    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
	        return false;

	    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
	    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
	    {
	        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
	        ::SetCursor(nullptr);
	    }
	    else
	    {
	        // Show OS mouse cursor
	        LPTSTR win32_cursor = IDC_ARROW;
	        switch (imgui_cursor)
	        {
	        case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
	        case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
	        case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
	        case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
	        case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
	        case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
	        case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
	        case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
	        case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
	        }
	        ::SetCursor(::LoadCursor(nullptr, win32_cursor));
	    }
	    return true;
	}

	void AddKeyEvent(ImGuiKey key, bool down, int native_keycode, int native_scancode)
	{
	    ImGuiIO& io = ImGui::GetIO();
	    io.AddKeyEvent(key, down);
	    io.SetKeyEventNativeData(key, native_keycode, native_scancode); // To support legacy indexing (<1.87 user code)
	    IM_UNUSED(native_scancode);
	}

	ImGuiKey VirtualKeyToImGuiKey(WPARAM wParam)
	{
	    switch (wParam)
	    {
	        case VK_TAB: return ImGuiKey_Tab;
	        case VK_LEFT: return ImGuiKey_LeftArrow;
	        case VK_RIGHT: return ImGuiKey_RightArrow;
	        case VK_UP: return ImGuiKey_UpArrow;
	        case VK_DOWN: return ImGuiKey_DownArrow;
	        case VK_PRIOR: return ImGuiKey_PageUp;
	        case VK_NEXT: return ImGuiKey_PageDown;
	        case VK_HOME: return ImGuiKey_Home;
	        case VK_END: return ImGuiKey_End;
	        case VK_INSERT: return ImGuiKey_Insert;
	        case VK_DELETE: return ImGuiKey_Delete;
	        case VK_BACK: return ImGuiKey_Backspace;
	        case VK_SPACE: return ImGuiKey_Space;
	        case VK_RETURN: return ImGuiKey_Enter;
	        case VK_ESCAPE: return ImGuiKey_Escape;
	        case VK_OEM_7: return ImGuiKey_Apostrophe;
	        case VK_OEM_COMMA: return ImGuiKey_Comma;
	        case VK_OEM_MINUS: return ImGuiKey_Minus;
	        case VK_OEM_PERIOD: return ImGuiKey_Period;
	        case VK_OEM_2: return ImGuiKey_Slash;
	        case VK_OEM_1: return ImGuiKey_Semicolon;
	        case VK_OEM_PLUS: return ImGuiKey_Equal;
	        case VK_OEM_4: return ImGuiKey_LeftBracket;
	        case VK_OEM_5: return ImGuiKey_Backslash;
	        case VK_OEM_6: return ImGuiKey_RightBracket;
	        case VK_OEM_3: return ImGuiKey_GraveAccent;
	        case VK_CAPITAL: return ImGuiKey_CapsLock;
	        case VK_SCROLL: return ImGuiKey_ScrollLock;
	        case VK_NUMLOCK: return ImGuiKey_NumLock;
	        case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
	        case VK_PAUSE: return ImGuiKey_Pause;
	        case VK_NUMPAD0: return ImGuiKey_Keypad0;
	        case VK_NUMPAD1: return ImGuiKey_Keypad1;
	        case VK_NUMPAD2: return ImGuiKey_Keypad2;
	        case VK_NUMPAD3: return ImGuiKey_Keypad3;
	        case VK_NUMPAD4: return ImGuiKey_Keypad4;
	        case VK_NUMPAD5: return ImGuiKey_Keypad5;
	        case VK_NUMPAD6: return ImGuiKey_Keypad6;
	        case VK_NUMPAD7: return ImGuiKey_Keypad7;
	        case VK_NUMPAD8: return ImGuiKey_Keypad8;
	        case VK_NUMPAD9: return ImGuiKey_Keypad9;
	        case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
	        case VK_DIVIDE: return ImGuiKey_KeypadDivide;
	        case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
	        case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
	        case VK_ADD: return ImGuiKey_KeypadAdd;
	        case IM_VK_KEYPAD_ENTER: return ImGuiKey_KeypadEnter;
	        case VK_LSHIFT: return ImGuiKey_LeftShift;
	        case VK_LCONTROL: return ImGuiKey_LeftCtrl;
	        case VK_LMENU: return ImGuiKey_LeftAlt;
	        case VK_LWIN: return ImGuiKey_LeftSuper;
	        case VK_RSHIFT: return ImGuiKey_RightShift;
	        case VK_RCONTROL: return ImGuiKey_RightCtrl;
	        case VK_RMENU: return ImGuiKey_RightAlt;
	        case VK_RWIN: return ImGuiKey_RightSuper;
	        case VK_APPS: return ImGuiKey_Menu;
	        case '0': return ImGuiKey_0;
	        case '1': return ImGuiKey_1;
	        case '2': return ImGuiKey_2;
	        case '3': return ImGuiKey_3;
	        case '4': return ImGuiKey_4;
	        case '5': return ImGuiKey_5;
	        case '6': return ImGuiKey_6;
	        case '7': return ImGuiKey_7;
	        case '8': return ImGuiKey_8;
	        case '9': return ImGuiKey_9;
	        case 'A': return ImGuiKey_A;
	        case 'B': return ImGuiKey_B;
	        case 'C': return ImGuiKey_C;
	        case 'D': return ImGuiKey_D;
	        case 'E': return ImGuiKey_E;
	        case 'F': return ImGuiKey_F;
	        case 'G': return ImGuiKey_G;
	        case 'H': return ImGuiKey_H;
	        case 'I': return ImGuiKey_I;
	        case 'J': return ImGuiKey_J;
	        case 'K': return ImGuiKey_K;
	        case 'L': return ImGuiKey_L;
	        case 'M': return ImGuiKey_M;
	        case 'N': return ImGuiKey_N;
	        case 'O': return ImGuiKey_O;
	        case 'P': return ImGuiKey_P;
	        case 'Q': return ImGuiKey_Q;
	        case 'R': return ImGuiKey_R;
	        case 'S': return ImGuiKey_S;
	        case 'T': return ImGuiKey_T;
	        case 'U': return ImGuiKey_U;
	        case 'V': return ImGuiKey_V;
	        case 'W': return ImGuiKey_W;
	        case 'X': return ImGuiKey_X;
	        case 'Y': return ImGuiKey_Y;
	        case 'Z': return ImGuiKey_Z;
	        case VK_F1: return ImGuiKey_F1;
	        case VK_F2: return ImGuiKey_F2;
	        case VK_F3: return ImGuiKey_F3;
	        case VK_F4: return ImGuiKey_F4;
	        case VK_F5: return ImGuiKey_F5;
	        case VK_F6: return ImGuiKey_F6;
	        case VK_F7: return ImGuiKey_F7;
	        case VK_F8: return ImGuiKey_F8;
	        case VK_F9: return ImGuiKey_F9;
	        case VK_F10: return ImGuiKey_F10;
	        case VK_F11: return ImGuiKey_F11;
	        case VK_F12: return ImGuiKey_F12;
	        default: return ImGuiKey_None;
	    }
	}

	BOOL IsWindowsVersionOrGreater(WORD major, WORD minor, WORD)
	{
	    typedef LONG(WINAPI* PFN_RtlVerifyVersionInfo)(OSVERSIONINFOEXW*, ULONG, ULONGLONG);
	    static PFN_RtlVerifyVersionInfo RtlVerifyVersionInfoFn = nullptr;
		if (RtlVerifyVersionInfoFn == nullptr)
			if (HMODULE ntdllModule = ::GetModuleHandleA("ntdll.dll"))
				RtlVerifyVersionInfoFn = (PFN_RtlVerifyVersionInfo)GetProcAddress(ntdllModule, "RtlVerifyVersionInfo");
	    if (RtlVerifyVersionInfoFn == nullptr)
	        return FALSE;

	    RTL_OSVERSIONINFOEXW versionInfo = { };
	    ULONGLONG conditionMask = 0;
	    versionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);
	    versionInfo.dwMajorVersion = major;
		versionInfo.dwMinorVersion = minor;
		VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
		VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
		return (RtlVerifyVersionInfoFn(&versionInfo, VER_MAJORVERSION | VER_MINORVERSION, conditionMask) == 0) ? TRUE : FALSE;
	}

	float GetDpiScaleForMonitor(void* monitor)
	{
	    UINT xdpi = 96, ydpi = 96;
	    if (IsWindowsVersionOrGreater(HIBYTE(0x0603), LOBYTE(0x0603), 0) )
	    {
			static HINSTANCE shcore_dll = ::LoadLibraryA("shcore.dll"); // Reference counted per-process
			static PFN_GetDpiForMonitor GetDpiForMonitorFn = nullptr;
			if (GetDpiForMonitorFn == nullptr && shcore_dll != nullptr)
	            GetDpiForMonitorFn = (PFN_GetDpiForMonitor)::GetProcAddress(shcore_dll, "GetDpiForMonitor");
			if (GetDpiForMonitorFn != nullptr)
			{
				GetDpiForMonitorFn((HMONITOR)monitor, MONITOR_DPI_TYPE::MDT_EFFECTIVE_DPI, &xdpi, &ydpi);
	            IM_ASSERT(xdpi == ydpi); // Please contact me if you hit this assert!
				return xdpi / 96.0f;
			}
	    }
#ifndef NOGDI
    const HDC dc = ::GetDC(nullptr);
    xdpi = ::GetDeviceCaps(dc, LOGPIXELSX);
    ydpi = ::GetDeviceCaps(dc, LOGPIXELSY);
    IM_ASSERT(xdpi == ydpi); // Please contact me if you hit this assert!
    ::ReleaseDC(nullptr, dc);
#endif
		return xdpi / 96.0f;
	}

	BOOL CALLBACK UpdateMonitors_EnumFunc(HMONITOR monitor, HDC, LPRECT, LPARAM)
	{
	    MONITORINFO info = {};
	    info.cbSize = sizeof(MONITORINFO);
	    if (!::GetMonitorInfo(monitor, &info))
	        return TRUE;
	    ImGuiPlatformMonitor imgui_monitor;
	    imgui_monitor.MainPos = ImVec2((float)info.rcMonitor.left, (float)info.rcMonitor.top);
	    imgui_monitor.MainSize = ImVec2((float)(info.rcMonitor.right - info.rcMonitor.left), (float)(info.rcMonitor.bottom - info.rcMonitor.top));
	    imgui_monitor.WorkPos = ImVec2((float)info.rcWork.left, (float)info.rcWork.top);
	    imgui_monitor.WorkSize = ImVec2((float)(info.rcWork.right - info.rcWork.left), (float)(info.rcWork.bottom - info.rcWork.top));
	    imgui_monitor.DpiScale = GetDpiScaleForMonitor(monitor);
	    imgui_monitor.PlatformHandle = (void*)monitor;
	    ImGuiPlatformIO& io = ImGui::GetPlatformIO();
	    if (info.dwFlags & MONITORINFOF_PRIMARY)
	        io.Monitors.push_front(imgui_monitor);
	    else
	        io.Monitors.push_back(imgui_monitor);
	    return TRUE;
	}

}
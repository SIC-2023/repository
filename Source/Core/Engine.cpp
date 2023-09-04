#include "Engine.h"

#include <iostream>

#include "../Utility/Misc.h"
#include "../Core/Window.h"
#include "../Core/Timer.h"
#include "../Input/InputManager.h"
#include "../Graphics/GraphicsEngine.h"
#include "../Scene/SceneManager.h"

namespace argent
{
	Engine* engine = nullptr;

	Engine* GetEngine()
	{
		return engine;
	}

	Engine::Engine():
		is_request_showdown_(false)
	{
		if (engine) _ASSERT_EXPR(FALSE, L"�G���W���̃C���X�^���X��������`����Ă��܂�");

		engine = this;

		std::cout << "�G���W�����쐬����܂���" << std::endl;	
	}

	Engine::~Engine()
	{
		//TODO subsystemLocator��Remove���쐬����
		
	}

	void Engine::SetUp()
	{
		//TODO �e��T�u�V�X�e����ǉ����Ă���
		subsystem_locator_.Add<Window>();

		subsystem_locator_.Add<input::InputManager>();

		subsystem_locator_.Add<graphics::GraphicsEngine>();
		subsystem_locator_.Add<scene::SceneManager>();
		subsystem_locator_.Add<Timer>();
	}

	void Engine::OnShutdown()
	{
		//TODO
		subsystem_locator_.Remove<Timer>();
		subsystem_locator_.Remove<scene::SceneManager>();
		subsystem_locator_.Remove<graphics::GraphicsEngine>();

		subsystem_locator_.Remove<input::InputManager>();

		subsystem_locator_.Remove<Window>();
	}

	void Engine::Run()
	{
		//TODO �V�[���}�l�[�W���ǂ񂾂�Ƃ�

		//�E�B���h�E�C�x���g�A���b�Z�[�W�̍X�V
		const auto& window = subsystem_locator_.Get<Window>();
		window->ProcessSystemEventQueue();
		

		//�L�[���͂̍X�V
		subsystem_locator_.Get<input::InputManager>()->Update(window->GetHwnd());

		//�^�C�}�[�X�V
		subsystem_locator_.Get<Timer>()->Tick();

		//�V�[���A�b�v�f�[�g
		const auto scene_manager = subsystem_locator_.Get<scene::SceneManager>();
		scene_manager->Update();

		//�`��
		const auto gfx = subsystem_locator_.Get<graphics::GraphicsEngine>();

		const auto render_context = gfx->Begin();

		scene_manager->Render(render_context);

		gfx->End(render_context);
	}
}

#pragma once
#include <Windows.h>

#include <memory>

#include "../Graphics/GraphicsEngine.h"
#include "../Scene/SceneManager.h"
#include "Timer.h"

#include "Engine.h"

namespace argent
{
	//�A�v���P�[�V�����N���X
	//���C���֐����Ŏ��̂�錾���A�g�p����
	//
	class Application
	{
	public:
		Application() = default;

		//�R�s�[�y�у��[�u���֎~
		virtual ~Application() = default;
		Application(const Application&) = delete;
		Application(const Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(const Application&&) = delete;


		void SetUp();
		int OnShutdown();

		/**
		 * \brief �A�v���P�[�V�����̎��s�֐�
		 * \return �I���R�[�h
		 */
		void Run();

	private:


		std::unique_ptr<Engine> engine_;
	};
}
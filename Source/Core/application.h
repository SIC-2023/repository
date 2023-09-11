#pragma once
#include <memory>

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


		void SetUp(bool is_editor_mode = true);
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
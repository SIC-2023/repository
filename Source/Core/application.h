#pragma once
#include <memory>

#include "Engine.h"

namespace argent
{
	//アプリケーションクラス
	//メイン関数内で実体を宣言し、使用する
	//
	class Application
	{
	public:
		Application() = default;

		//コピー及びムーブを禁止
		virtual ~Application() = default;
		Application(const Application&) = delete;
		Application(const Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(const Application&&) = delete;


		void SetUp(bool is_editor_mode = true);
		int OnShutdown();

		/**
		 * \brief アプリケーションの実行関数
		 * \return 終了コード
		 */
		void Run();

	private:

		std::unique_ptr<Engine> engine_;
	};
}
#pragma once

namespace argent
{
	/**
	 * \brief float型の乱数を生成
	 * \param min 最小値
	 * \param max 最大値
	 * \return 生成された乱数
	 */
	float GenerateFRand(float min, float max);

	/**
	 * \brief int型の乱数を生成
	 * \param min 最小値
	 * \param max 最大値
	 * \return 生成された乱数
	 */
	int GenerateIRand(int min, int max);
}


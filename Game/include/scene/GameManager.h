#pragma once
#include "Game.h"

class GameManager
{
public:
	GameManager() {
		GameStart();
	}

	/// <summary>
	/// 最大ラウンド数を設定
	/// </summary>
	void SetMaxRound(int maxRound) {
		m_maxRound = maxRound;
	}

	/// <summary>
	/// ラウンドの終了
	/// </summary>
	/// <param name="isDeathPlayer">プレイヤーが死んでいるか</param>
	void GameEnd(bool isDeathPlayer[PLAYER_NUM]);

	/// <summary>
	/// 制限時間設定を取得
	/// </summary>
	int GetTimeLimitSec()const {
		return m_timeLimitSec;
	}

	/// <summary>
	/// 現在のラウンド数を取得
	/// </summary>
	int GetRoundCount()const {
		return m_roundCount;
	}
	/// <summary>
	/// 最大ラウンド数を取得
	/// </summary>
	int GetMaxRound()const {
		return m_maxRound;
	}

	/// <summary>
	/// プレイヤーのスコアを取得
	/// </summary>
	int GetPlayerScore(int playernum)const {
		return m_score[playernum];
	}

private:
	void GameStart();

private:
	std::unique_ptr<Game> m_game;

	int m_roundCount = 0;//ラウンド数
	int m_score[PLAYER_NUM] = {};//スコア

	int m_maxRound = 5;//最大ラウンド
	int m_timeLimitSec = 30;//制限時間(秒)
};


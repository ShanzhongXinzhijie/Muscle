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
	/// 現在のラウンドが練習ラウンドか取得
	/// </summary>
	bool GetIsPracticeRound()const {
		return m_isPracticeRound;
	}

	/// <summary>
	/// 現在のラウンド数を取得
	/// </summary>
	int GetRoundCount()const {
		return m_isPracticeRound ? -1 : m_roundCount;
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

	bool m_isPracticeRound = true;//次に開始するのが練習ラウンドか
	int m_roundCount = 0;//ラウンド数
	int m_score[PLAYER_NUM] = {};//スコア

	static constexpr int PRACTICE_TIME_LIMIT = 120;//練習ラウンド制限時間
	static constexpr int TIME_LIMIT = 30;//制限時間

	int m_maxRound = 5;//最大ラウンド
	int m_timeLimitSec = PRACTICE_TIME_LIMIT;//制限時間(秒)
};


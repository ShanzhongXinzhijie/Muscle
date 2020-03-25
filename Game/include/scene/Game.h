#pragma once
#include"Shibuya.h"
#include"CPlayer.h"
#include"CountDown.h"

class GameManager;

class Game : public IGameObject
{
public:
	Game(GameManager* manager);
	~Game() {
		//生成物削除
		ICreateObject::AllDestroy();
	}

	void PreUpdate()override;
	void PostLoopUpdate()override;
	void PostRender()override;

private:
	//プレイヤー数
	int m_playerNum = 1;

	//プレイヤー
	std::unique_ptr<CPlayer> m_player[PLAYER_NUM];
	//ステージ
	std::unique_ptr<Shibuya> m_shibuya;

	//制限時間
	float m_timeLimitSec = 15.0f;
	//終了までの時間
	float m_toEndTimeSec = 0.0f;

	//フォント
	HUDFont m_font;
	CFont m_bigFont;

	//ゲームマネージャー
	GameManager* m_manager = nullptr;

	//終了ボタン
	int m_pushCnt = 0;
	int MAX_PUSH = 20;
	int m_pushCntNoCon = 0;
	int MAX_PUSH_NOCON = 20;
};


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
	void PostRender()override;

private:
	//プレイヤー
	std::unique_ptr<CPlayer> m_player[PLAYER_NUM];
	//ステージ
	std::unique_ptr<Shibuya> m_shibuya;

	//制限時間
	float m_timeLimitSec = 15.0f;

	//フォント
	HUDFont m_font;

	//ゲームマネージャー
	GameManager* m_manager = nullptr;

	int m_pushCnt = 0;
	int MAX_PUSH = 20;
	int m_pushCntNoCon = 0;
	int MAX_PUSH_NOCON = 20;
};


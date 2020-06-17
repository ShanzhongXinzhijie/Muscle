#include "stdafx.h"
#include "Game.h"
#include "GameManager.h"
#include "CGameMode.h"

Game::Game(GameManager* manager) 
	: m_manager(manager), m_timeLimitSec(static_cast<float>(manager->GetTimeLimitSec()) + 0.9f)
{
	//SEに参照を渡す
	GameSE::SetGamePtr(this);

	//カウントダウンクラス生成
	int scores[PLAYER_NUM] = { m_manager->GetPlayerScore(0),m_manager->GetPlayerScore(1) };
	new CountDown(m_manager->GetRoundCount(), m_manager->GetMaxRound(), scores, m_manager->GetTimeLimitSec());

	//プレイヤー生成
	m_player[0] = std::make_unique<CPlayer>(1, m_manager->GetIsPracticeRound());
	m_player[1] = std::make_unique<CPlayer>(0, m_manager->GetIsPracticeRound());

	//草
	g_grassRunner.Init();

	//練習ラウンドなら
	if (m_manager->GetIsPracticeRound()) {
		for (auto& player : m_player) {
			//不死設定
			player->GetDeathHotoke().SetIsImmortal(true);
			//操作説明表示
			player->GetDeathHotoke().SetIsDrawHowto(true);
		}
	}

	//ステージ生成
	m_shibuya = std::make_unique<Shibuya>();

	//フォント初期化
	//m_font.SetColor(CVector4::Black());
	m_bigFont.LoadFont(L"Resource/font/eunomia_0200/EunomiaBold.spritefont");

	//プレイヤー人数の取得
	m_playerNum = FindGO<CGameMode>(L"CGameMode")->GetPlayerNum();
	if (m_playerNum == 1) {
		MAX_PUSH = 6;
		MAX_PUSH_NOCON = 6;
	}
}

void Game::PreUpdate() {
	if (m_toEndTimeSec < FLT_EPSILON) {
		//プレイヤーの死亡を検知
		bool isDeath = false;
		bool isDeathPlayer[PLAYER_NUM] = {};
		for (int i = 0; i < PLAYER_NUM; i++) {
			if (m_player[i]->GetIsDeath()) {
				isDeath = true;
				isDeathPlayer[m_player[i]->GetPlayerNum()] = true;
			}
		}
		//プレイヤー死亡でゲーム終了
		if (isDeath) {
			//秒後にラウンド終了
			m_toEndTimeSec = 2.3f;
			//スローモーション
			TimeManager::GetInstance().SetFrameRate(1.4f, 6, 1.0f);
			//SE
			SuicideObj::CSE* se = NewGO<SuicideObj::CSE>(L"Resource/sound/Pickup_Coin2.wav");
			se->Play();
			//無敵化
			for (auto& player : m_player) {
				player->GetDeathHotoke().SetIsImmortal(true);
			}
		}

		//制限時間
		m_timeLimitSec -= FRAME_PER_SECOND;
		if (m_timeLimitSec < 0.0f) {
			//秒後にラウンド終了
			m_toEndTimeSec = 2.3f;
			//スローモーション
			TimeManager::GetInstance().SetFrameRate(1.4f, 6, 1.0f);
			//SE
			SuicideObj::CSE* se = NewGO<SuicideObj::CSE>(L"Resource/sound/Randomize.wav");
			se->Play();			
			//無敵化
			for (auto& player : m_player) {
				player->GetDeathHotoke().SetIsImmortal(true);
			}
			return;
		}
	}

	//練習ラウンドならラウンド終了操作あり
	if (m_manager->GetIsPracticeRound()) {
		for (int i = 0; i < PLAYER_NUM; i++) {
			if (Pad(i).GetDown(enButtonY)) {
				m_pushCnt++;
			}
		}
		if (m_pushCnt >= MAX_PUSH) {
			m_timeLimitSec = -1.0f;
		}		
	}

	//ノーコンテスト
	for (int i = 0; i < PLAYER_NUM; i++) {
		if (Pad(i).GetDown(enButtonB)) {
			m_pushCntNoCon++;
		}
	}
	if (m_pushCntNoCon >= MAX_PUSH_NOCON) {
		m_manager->NoContest();
		return;
	}
}

void Game::PostLoopUpdate() {
	if (m_toEndTimeSec > 0.0f) {
		m_toEndTimeSec -= GetRealDeltaTimeSec();
		if (m_toEndTimeSec < FLT_EPSILON) {
			//プレイヤーの死亡を検知
			bool isDeath = false;
			bool isDeathPlayer[PLAYER_NUM] = {};
			for (int i = 0; i < PLAYER_NUM; i++) {
				if (m_player[i]->GetIsDeath()) {
					isDeath = true;
					isDeathPlayer[m_player[i]->GetPlayerNum()] = true;
				}
			}
			//終了
			m_manager->GameEnd(isDeathPlayer);
			return;
		}
	}
}

namespace {
	constexpr wchar_t TEXT[][64] = { L"[B]ノーコンテスト(%d/%d)\n\nれんしゅうラウンド\n[Y]れんだでゲームスタート(%d/%d)", L"[B]ノーコンテスト(%d/%d)" };
}

void Game::PostRender() {
	//制限時間
	m_timeLimitSec = max(0.0f, m_timeLimitSec);

	if (GetScreenNum() == 1) {
		m_font.DrawFormat(
			L"%2d:%2d",
			{ 0.95f,0.0f }, { 1.0f,0.0f },
			static_cast<int>(m_timeLimitSec / 60.0f), static_cast<int>(m_timeLimitSec) % 60
		);
	}
	else {
		m_font.DrawFormat(
			L"%2d:%2d",
			{ 0.5f,0.0f }, { 0.5f,0.0f },
			//m_manager->GetRoundCount(), m_manager->GetMaxRound(),
			//m_manager->GetPlayerScore(0), m_manager->GetPlayerScore(1),
			static_cast<int>(m_timeLimitSec / 60.0f), static_cast<int>(m_timeLimitSec) % 60
		);
	}

	//ラウンド終了操作
	int ind = m_manager->GetIsPracticeRound() ? 0 : 1;
	CVector2 scale = m_font.GetScale();
	m_font.SetScale(scale*0.5f);
	m_font.SetUseFont(m_font.enJPN);
	if (GetScreenNum() == 1) {
		m_font.DrawFormat(
			TEXT[ind],
			{ 0.95f,0.08f }, { 1.0f,0.0f },
			m_pushCntNoCon, MAX_PUSH_NOCON,
			m_pushCnt, MAX_PUSH
		);
	}
	else {
		m_font.DrawFormat(
			TEXT[ind],
			{ 0.5f,0.08f }, { 0.5f,0.0f },
			m_pushCntNoCon, MAX_PUSH_NOCON,
			m_pushCnt, MAX_PUSH
		);
	}
	m_font.SetUseFont(m_font.enENG);
	m_font.SetScale(scale);

	//勝敗表示
	if (m_toEndTimeSec > 0.0f) {
		//プレイヤーの死亡を検知
		bool isDeathPlayer[PLAYER_NUM] = {};
		for (int i = 0; i < PLAYER_NUM; i++) {
			if (m_player[i]->GetIsDeath()) {
				isDeathPlayer[m_player[i]->GetPlayerNum()] = true;
			}
		}
		wchar_t string[64];
		if (isDeathPlayer[0] && isDeathPlayer[1]) {
			wcscpy_s(string, L"destroyed");
		}
		else if (isDeathPlayer[0]){
			if (m_playerNum == 1) {
				wcscpy_s(string, L"YOU\nLOSE");
			}
			else {
				wcscpy_s(string, L"Right Side\nWIN");
			}
		}
		else if (isDeathPlayer[1]) {
			if (m_playerNum == 1) {
				wcscpy_s(string, L"YOU\nWIN");
			}
			else {
				wcscpy_s(string, L"Left Side\nWIN");
			}
		}
		else {
			wcscpy_s(string, L"TIME UP");
		}
		m_bigFont.Draw(string, 0.5f, CVector4::Black(), 1.0f, { 0.575f, 0.6f });
	}
}

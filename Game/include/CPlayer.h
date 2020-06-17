#pragma once
#include"IGamePad.h"
#include"CDeathHotoke.h"
#include"HotokeCamera.h"
#include"ZoomOutCamera.h"
#include"HUDFont.h"
#include"TreeGene.h"
#include"AssembleScene.h"
#include"AI.h"

class CGameMode;

class HumanPlayer : public IGameObject {
public:
	HumanPlayer(int playernum, CDeathHotoke& hotoke);

	//IGameObject
	bool Start()override;
	void Update()override;
	void PostUpdate()override;
	void PostLoopUpdate()override;
	void HUDRender(int HUDNum)override;

	IGamePad& GetPad() {
		return m_pad;
	}

	HUDFont& GetHUDFont() {
		return m_HUDFont;
	}

private:
	int m_playerNum = -1;

	//ホトケ参照
	CDeathHotoke& m_hotoke;

	//カメラ
	HotokeCameraController m_cam;

	//パッド
	IGamePad m_pad;

	//人間参照
	CHuman* m_humanPtr = nullptr;

	//HUD
	bool m_isDrawHUD = true;
	bool m_isLockon = false;
	float m_targetHP = 0.f;
	CVector3 m_targetPos;
	CVector4 m_HUDColor = { 0.0f,0.0f,0.0f,1.0f };
	CVector4 m_enemyColor = { 0.0f,1.0f,0.0f,1.0f };
	CSprite m_guncross, m_wMark, m_velocityVector;
	//CVector3 m_guncrossPosOld, m_velocityPosOld;
	HUDFont m_HUDFont, m_warningFont, m_japaneseFont;
};

class CPlayer : public IGameObject
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="playernum">プレイヤー番号</param>
	/// <param name="isNoAI">AIを使用しない</param>
	CPlayer(int playernum, bool isNoAI = false);

	//IGameObject
	bool Start()override;

	/// <summary>
	/// プレイヤー番号を取得
	/// </summary>
	int GetPlayerNum()const {
		return m_playerNum;
	}

	/// <summary>
	/// 死んでる?
	/// </summary>
	bool GetIsDeath()const {
		return m_hotoke.GetHP() <= 0.0f;
	}

	/// <summary>
	/// デスホトケ本体を取得
	/// </summary>
	CDeathHotoke& GetDeathHotoke() {
		return m_hotoke;
	}

private:
	//プレイヤー番号
	int m_playerNum = -1;

	//人間用データ
	std::unique_ptr<HumanPlayer> m_humanPlayer;
		
	HotokeAssembleManager::HotokeAssemble& m_assemble;//アセンブル設定
	CDeathHotoke m_hotoke;//ホトケ本体
};


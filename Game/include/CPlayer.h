#pragma once
#include"IGamePad.h"
#include"CDeathHotoke.h"
#include"HotokeCamera.h"
#include"ZoomOutCamera.h"
#include"HUDFont.h"
#include"TreeGene.h"
#include"AssembleScene.h"
#include"AI.h"

class CPlayer : public IGameObject
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="playernum">プレイヤー番号</param>
	CPlayer(int playernum);

	//IGameObject
	bool Start()override;
	void Update()override;
	void PostUpdate()override;
	void PostLoopUpdate()override;
	void HUDRender(int HUDNum)override;

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

private:
	//プレイヤー番号
	int m_playerNum = -1;
		
	HotokeAssembleManager::HotokeAssemble& m_assemble;//アセンブル設定
	CDeathHotoke m_hotoke;//ホトケ本体
	
	//カメラ
	std::unique_ptr<HotokeCameraController> m_cam;
	//ZoomOutCamera m_zoomoutCam;
	
	//パッド
	std::unique_ptr<IGamePad> m_pad;

	//草
	Grass m_grass[Grass::m_sInstancingMax / PLAYER_NUM];

	//テスト用モデル
	AnimationClip m_anim, m_animHeri;
	GameObj::CSkinModelRender m_human, m_heri;
	GameObj::PerspectiveCamera m_humanCam;

	//HUD
	bool m_isDrawHUD = true;
	bool m_isLockon = false;
	float m_targetHP = 0.f;
	CVector3 m_targetPos;
	CVector4 m_HUDColor = { 0.0f,0.0f,0.0f,1.0f };
	CSprite m_guncross, m_wMark, m_velocityVector;
	//CVector3 m_guncrossPosOld, m_velocityPosOld;
	HUDFont m_HUDFont, m_warningFont, m_japaneseFont;
};


#pragma once
#include"IGamePad.h"
#include"CDeathHotoke.h"
#include"HotokeCamera.h"
#include"ZoomOutCamera.h"
#include"HUDFont.h"
#include"TreeGene.h"

class CPlayer : public IGameObject
{
public:
	CPlayer(int padnum)
		:m_playerNum(padnum), m_pad(padnum),m_hotoke(padnum,&m_pad,true,&m_HUDFont,nullptr),m_cam(&m_hotoke, &m_pad),
		m_HUDFont(m_HUDColor, 0.5f), m_warningFont({1.0f,0.0f,0.0f,1.0f}, 0.5f), m_japaneseFont(m_HUDColor, 0.5f)
	{
		//メインカメラ設定
		m_cam.SetToMainCamera(padnum);

		//草
		for (auto& grass : m_grass) {
			grass.SetDrawCameraNum(m_playerNum);
		}

		//フォント
		m_japaneseFont.SetUseFont(HUDFont::enJPN);
		m_hotoke.SetFonts(&m_warningFont, &m_japaneseFont);
	};

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
	int m_playerNum = -1;

	IGamePad m_pad;
	CDeathHotoke m_hotoke;
	
	//カメラ
	HotokeCameraController m_cam;
	//ZoomOutCamera m_zoomoutCam;
	GameObj::PerspectiveCamera m_humanCam;

	//草
	Grass m_grass[Grass::m_sInstancingMax / PLAYER_NUM];

	//テスト用モデル
	AnimationClip m_anim, m_animHeri;
	GameObj::CSkinModelRender m_human, m_heri;

	//HUD
	bool m_isDrawHUD = true;
	bool m_isLockon = false;
	CVector4 m_HUDColor = { 0.0f,0.0f,0.0f,1.0f };
	CSprite m_guncross, m_wMark, m_velocityVector;
	//CVector3 m_guncrossPosOld, m_velocityPosOld;
	HUDFont m_HUDFont, m_warningFont, m_japaneseFont;
};


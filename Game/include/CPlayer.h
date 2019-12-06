#pragma once
#include"IGamePad.h"
#include"CDeathHotoke.h"
#include"HotokeCamera.h"
#include"ZoomOutCamera.h"
#include"HUDFont.h"

class CPlayer : public IGameObject
{
public:
	CPlayer(int padnum)
		:m_playerNum(padnum), m_pad(padnum),m_hotoke(padnum,&m_pad,true,&m_HUDFont,nullptr),m_cam(&m_hotoke, &m_pad),m_HUDFont(m_HUDColor,0.5f)
	{
		m_cam.SetToMainCamera(padnum);
		/*if (padnum != 0) {
			m_isDrawHUD = false;
			m_hotoke.SetIsDrawHUD(m_isDrawHUD);
		}*/
	};

	bool Start()override;
	void Update()override;
	void PostUpdate()override;
	void PostLoopUpdate()override;
	void HUDRender(int HUDNum)override;

private:
	int m_playerNum = -1;

	IGamePad m_pad;
	CDeathHotoke m_hotoke;
	
	HotokeCameraController m_cam;
	//ZoomOutCamera m_zoomoutCam;
	GameObj::PerspectiveCamera m_humanCam;

	AnimationClip m_anim;
	GameObj::CSkinModelRender m_human;

	//HUD
	bool m_isDrawHUD = true;
	bool m_isLockon = false;
	CVector4 m_HUDColor = { 0.0f,0.0f,0.0f,1.0f };
	CSprite m_guncross, m_wMark, m_velocityVector;
	//CVector3 m_guncrossPosOld, m_velocityPosOld;
	HUDFont m_HUDFont;
};


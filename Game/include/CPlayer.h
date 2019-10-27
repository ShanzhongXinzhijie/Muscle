#pragma once
#include"IGamePad.h"
#include"CDeathHotoke.h"
#include"HotokeCamera.h"

class CPlayer : public IGameObject
{
public:
	CPlayer(int padnum):m_pad(padnum),m_hotoke(&m_pad,true,nullptr),m_cam(&m_hotoke, &m_pad){
		m_cam.SetToMainCamera(padnum);
		if (padnum != 0) {
			m_isDrawHUD = false;
			m_hotoke.SetIsDrawHUD(m_isDrawHUD);
		}
	};

	bool Start()override;
	void Update()override;
	void PostLoopUpdate()override;
	void PostRender()override;

private:
	IGamePad m_pad;
	CDeathHotoke m_hotoke;
	
	HotokeCameraController m_cam;
	GameObj::PerspectiveCamera m_humanCam;

	AnimationClip m_anim;
	GameObj::CSkinModelRender m_human;

	//HUD
	bool m_isDrawHUD = true;
	bool m_isLockon = false;
	CVector4 m_HUDColor = { 0.0f,0.0f,0.0f,1.0f };
	CSprite m_guncross, m_wMark, m_velocityVector;
};


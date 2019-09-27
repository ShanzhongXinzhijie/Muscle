#pragma once
#include"IGamePad.h"
#include"CDeathHotoke.h"
#include"HotokeCamera.h"

class CPlayer : public IGameObject
{
public:
	CPlayer(int padnum):m_pad(padnum),m_hotoke(&m_pad,true,nullptr),m_cam(&m_hotoke, &m_pad){
		m_cam.SetToMainCamera(padnum);
	};

	bool Start()override;
	void Update()override;

private:
	IGamePad m_pad;
	CDeathHotoke m_hotoke;
	
	HotokeCameraController m_cam;
	GameObj::PerspectiveCamera m_humanCam;

	AnimationClip m_anim;
	GameObj::CSkinModelRender m_human;
};


#pragma once
#include"IGamePad.h"
#include"CDeathHotoke.h"
#include"HotokeCamera.h"

class CPlayer : public IGameObject
{
public:
	CPlayer():m_hotoke(&m_pad){};

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


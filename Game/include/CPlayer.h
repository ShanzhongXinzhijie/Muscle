#pragma once
#include"CDeathHotoke.h"
#include"CPlayerCamera.h"

class CPlayer : public IGameObject
{
public:
	CPlayer();
	~CPlayer();

	bool Start()override;
	void Update()override;

private:
	CDeathHotoke m_hotoke;
	CPlayerCamera m_cam;

	AnimationClip m_anim;
	GameObj::CSkinModelRender m_human;
};


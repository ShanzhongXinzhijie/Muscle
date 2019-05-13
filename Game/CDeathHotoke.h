#pragma once
#include"CPlayerCamera.h"

class CDeathHotoke :
	public IGameObject
{
public:
	CDeathHotoke();
	~CDeathHotoke();

	bool Start()override;
	void Update()override;

private:
	GameObj::CSkinModelRender m_model[5];
	CPlayerCamera m_cam;

	CVector3 m_pos;
};


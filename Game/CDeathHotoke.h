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

	const CVector3& GetPos()const { return m_pos; }

private:
	GameObj::CSkinModelRender m_model[5];
	AnimationClip m_animationClip[2];
	CPlayerCamera m_cam;

	SuicideObj::CCollisionObj m_col;

	CVector3 m_pos;
};


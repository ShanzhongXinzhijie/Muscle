#pragma once
#include "IFu.h"
#include "CDeathHotoke.h"

class Ari :
	public IGameObject, public IFu
{
public:
	Ari(const CVector3& pos, const CQuaternion& rot) {
		SetPos(pos); SetRot(rot);
	}

	//UŒ‚‘ÎÛ‚ğİ’è
	void SetTarget(IFu* target) {
		m_target = target;
	}

	bool Start()override;
	void Update()override;

private:
	GameObj::CSkinModelRender m_model;
	AnimationClip m_animation;

	int m_moveCnt = 0;
	CVector3 m_velocity;
	CQuaternion m_shake;

	IFu* m_target = nullptr;
};


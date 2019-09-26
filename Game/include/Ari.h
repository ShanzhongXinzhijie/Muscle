#pragma once
#include "IFu.h"
#include "CDeathHotoke.h"

class Ari :
	public ILifeObject
{
public:
	Ari(const CVector3& pos, const CQuaternion& rot) {
		SetPos(pos); SetRot(rot);
	}

	//çUåÇëŒè€Çê›íË
	void SetTarget(IFu* target) {
		m_target = target;
	}

	bool Start()override;
	void Update()override;

private:
	void Damage(const ReferenceCollision& ref, const CVector3& pos);

private:
	GameObj::CSkinModelRender m_model;
	AnimationClip m_animation;

	float m_hp = 10.0f;

	int m_moveCnt = 0;
	CVector3 m_velocity;
	CQuaternion m_shake;

	IFu* m_target = nullptr;
};


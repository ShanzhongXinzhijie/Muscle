#pragma once
#include"BeamModel.h"

class BulletKani :
	public IGameObject
{
public:
	BulletKani(const CVector3& pos, const CVector3& move);

	void Update()override;

private:
	BeamModel m_model;
	float m_lifeTime = 3.0f;
	CVector3 m_pos, m_posOld, m_vector;
};

//TODO DecolatorパターンでBulletクラス作る
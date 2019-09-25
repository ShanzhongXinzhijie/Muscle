#pragma once
#include "IFu.h"
#include "BeamModel.h"

class CBlood :
	public IFu, public IGameObject
{
public:
	CBlood(const CVector3& pos, const CVector3& move)
		: m_posOld(pos), m_vector(move)
	{
		SetPos(pos);
	}

	bool Start()override;
	void Update()override;

private:
	//���f��
	BeamModel m_model;
	//����
	float m_lifeTime = 0.5f;
	//���W�Ƃ�
	CVector3 m_posOld, m_vector;
};


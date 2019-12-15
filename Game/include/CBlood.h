#pragma once
#include "IFu.h"
#include "BeamModel.h"

class CBlood :
	public IFu, public IGameObject
{
public:
	CBlood(const CVector3& pos, const CVector3& move, float radius = -1.0f, bool createCollision = true)
		: m_posOld(pos), m_vector(move), m_radiusSetting(radius), m_createCollision(createCollision)
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
	//����
	float m_lengthScale = 1.0f;
	//���a
	float m_radiusSetting = -1.0f;
	//�������x
	float m_gravityScale = 1.0f;
	//�R���W�������쐬���邩?
	bool m_createCollision = true;
};


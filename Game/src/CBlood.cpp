#include "stdafx.h"
#include "CBlood.h"

bool CBlood::Start() {
	//���f��
	m_model.Init(L"BLOOD",false);
	m_model.SetPos(m_posOld, GetPos());
	m_model.SetRadius(1.0f);

	//�ړ����������_��
	float speed = m_vector.Length(); 
	m_vector.Normalize();
	m_vector += CVector3(0.4f - 0.8f*CMath::RandomZeroToOne(), 0.4f - 0.8f*CMath::RandomZeroToOne(), 0.4f - 0.8f*CMath::RandomZeroToOne());
	m_vector.Normalize();
	m_vector *= speed;

	return true;
}

void CBlood::Update() {
	//�ړ�
	m_posOld = GetPos();
	Move(m_vector);

	//�ړ������ω�
	m_vector *= CVector3(1.2f - 0.4f*CMath::RandomZeroToOne(), 1.2f - 0.4f*CMath::RandomZeroToOne(), 1.2f - 0.4f*CMath::RandomZeroToOne());
	m_vector.y -= 7.0f;

	//��������
	m_lifeTime -= GetDeltaTimeSec();
	if (m_lifeTime < FLT_EPSILON) {
		delete this; return;
	}

	//���f���X�V
	m_model.SetPos(m_posOld, GetPos());
}
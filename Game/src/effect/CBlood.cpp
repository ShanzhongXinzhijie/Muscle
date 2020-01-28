#include "stdafx.h"
#include "CBlood.h"

bool CBlood::Start() {
	//���f��
	m_model.Init(L"BLOOD",false);
	m_model.SetPos(m_posOld, GetPos());
	if (m_radiusSetting > 0.0f) {
		m_model.SetRadius(m_radiusSetting);
	}
	else {
		m_model.SetRadius(0.5f + CMath::RandomZeroToOne());
	}

	//�ړ����������_��
	float speed = m_vector.Length(); 
	m_vector.Normalize();
	m_vector += CVector3(0.4f - 0.8f*CMath::RandomZeroToOne(), 0.4f - 0.8f*CMath::RandomZeroToOne(), 0.4f - 0.8f*CMath::RandomZeroToOne());
	m_vector.Normalize();
	m_vector *= speed;

	//����
	m_lengthScale = 0.1f + 1.2f*CMath::RandomZeroToOne();
	m_lengthScale = max(1.0f, m_lengthScale);
	//�������x
	m_gravityScale = 0.5f + 1.0f*CMath::RandomZeroToOne();

	return true;
}

void CBlood::Update() {
	//�ړ�
	m_posOld = GetPos();
	Move(m_vector);

	//�ړ������ω�
	m_vector *= CVector3(1.2f - 0.4f*CMath::RandomZeroToOne(), 1.2f - 0.4f*CMath::RandomZeroToOne(), 1.2f - 0.4f*CMath::RandomZeroToOne());
	m_vector.y -= 7.0f * m_gravityScale;

	//��������
	m_lifeTime -= FRAME_PER_SECOND;
	if (m_lifeTime < FLT_EPSILON) {
		delete this; return;
	}

	//�����蔻��
	if (m_createCollision) {
		//���C�Ŕ���
		btVector3 rayStart = m_posOld;
		btVector3 rayEnd = GetPos();
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			//���˕Ԃ�쐬
			for (int i = 0; i < 2; i++) {
				new CBlood(
					//�ʒu
					gnd_ray.m_hitPointWorld,
					//�ړ�
					gnd_ray.m_hitNormalWorld * (m_vector.Length()*0.5f),
					//���a
					m_model.GetRadius()*0.5f + (m_model.GetRadius()*0.2f*CMath::RandomZeroToOne()),
					//������Ȃ�
					false
				);
			}
			//TODO �����܂�
			//TODO
			//�t�\�t�g�p�[�e�B�N���̌���
		}
	}

	//���f���X�V
	m_model.SetPos(GetPos() + (m_posOld - GetPos())*m_lengthScale, GetPos());
}
#include "stdafx.h"
#include "BulletKani.h"

BulletKani::BulletKani(const CVector3& pos, const CVector3& move) : m_pos(pos), m_posOld(pos), m_vector(move) {
	m_model.Init();
	m_model.SetPos(m_posOld, m_pos);
	m_model.SetRadius(3.0f);

	//�����蔻��
	m_col.m_collision.CreateSphere(m_pos, {}, 3.0f);
	m_col.m_reference.damege = 1.0f;
	m_col.m_reference.attributes.set(enPhysical);
	m_col.m_reference.attributes.set(enFlame);
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//�N���X���o��
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				if (H->attributes[enPhysical]) {
					//���������Ȃ玀
					m_lifeTime = 0.0f;
				}
			}
		}
	);
}

void BulletKani::Update() {
	//�ړ�
	m_posOld = m_pos;
	m_pos += m_vector;

	//TODO �n�`����f�R���[�^�[

	//��������
	m_lifeTime -= GetDeltaTimeSec();
	if (m_lifeTime < FLT_EPSILON) {
		delete this; return;
	}

	//���f���X�V
	m_model.Move(m_vector);

	//�R���W�����X�V
	m_col.SetPos(m_pos);
}
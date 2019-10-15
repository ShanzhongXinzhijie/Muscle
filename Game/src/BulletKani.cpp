#include "stdafx.h"
#include "BulletKani.h"

BulletGO::BulletGO(IBulletDecolator* ptrDecolator, const CVector3& pos, const CVector3& move)
	: m_decolator(ptrDecolator), m_pos(pos), m_posOld(pos), m_vector(move)
{
	//������
	if(m_decolator)m_decolator->SetBullet(this);

	//���f��
	m_model.Init(L"BLUE");
	m_model.SetPos(m_posOld, m_pos);
	m_model.SetRadius(6.0f);
	m_lastDrawPos = m_pos;
	
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
			if (!p.m_isCCollisionObj) {
				//���肪CCollisionObj����Ȃ��Ă���
				m_lifeTime = 0.0f;
			}
		}
	);
	m_col.m_collision.SetIsCollisionStaticObject(true);//�ÓI�I�u�W�F�N�g�Ƃ��Փ˂���
	m_col.m_collision.SetIsHighSpeed(true);//����͍����ł�
}

void BulletGO::Update() {
	//m_isMoved = true;

	//�ړ�
	m_posOld = m_pos;
	m_pos += m_vector;

	//��������
	m_lifeTime -= GetDeltaTimeSec();
	if (m_lifeTime < FLT_EPSILON) {
		delete this; return;
	}

	//�R���W�����X�V
	m_col.SetPos(m_pos);
	m_col.SetDir(m_vector.GetNorm());
}

void BulletGO::PostLoopUpdate() {
	//���f���X�V
	if (!m_isMoved) {
		m_model.SetPos(m_posOld, m_pos); m_isMoved = true;
	}
	else {
		m_model.Move(m_pos - m_lastDrawPos);
	}
	m_lastDrawPos = m_pos;
}
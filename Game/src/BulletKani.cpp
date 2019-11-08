#include "stdafx.h"
#include "BulletKani.h"

BulletGO::BulletGO(const CVector3& pos, const CVector3& move)
	: m_vector(move)
{
	SetPos(pos), m_posOld = GetPos();

	//���f��
	m_model.Init(L"BLUE");
	m_model.SetPos(m_posOld, GetPos());
	m_model.SetRadius(3.0f);
	m_lastDrawPos = GetPos();
	
	//�����蔻��
	m_col.m_collision.CreateSphere(GetPos(), {}, 3.0f);
	m_col.m_reference.damege = 1.0f;
	m_col.m_reference.attributes.set(enPhysical);
	m_col.m_reference.attributes.set(enFlame);
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			for (auto& component : m_components) {
				component->Contact(p);
			}		
		}
	);
	m_col.m_collision.SetIsCollisionStaticObject(true);//�ÓI�I�u�W�F�N�g�Ƃ��Փ˂���
	m_col.m_collision.SetIsHighSpeed(true);//����͍����ł�
}

void BulletGO::Update() {
	//m_isMoved = true;

	for (auto& component : m_components) {
		component->Update();
	}

	//�ړ�
	m_posOld = GetPos();
	Move(m_vector);

	//��������
	m_lifeTime -= GetDeltaTimeSec();
	if (m_lifeTime < FLT_EPSILON) {
		delete this; return;
	}

	//�R���W�����X�V
	m_col.SetPos(GetPos());
	m_col.SetDir(m_vector.GetNorm());

	for (auto& component : m_components) {
		component->PostUpdate();
	}
}

void BulletGO::PostLoopUpdate() {
	//���f���X�V
	if (!m_isMoved) {
		m_model.SetPos(m_posOld, GetPos()); m_isMoved = true;
	}
	else {
		m_model.Move(GetPos() - m_lastDrawPos);
	}
	m_lastDrawPos = GetPos();
}
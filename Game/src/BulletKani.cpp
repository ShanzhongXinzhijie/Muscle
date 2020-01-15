#include "stdafx.h"
#include "BulletKani.h"
#include "CShockRing.h"

namespace {
	constexpr int BOOST_EFECT_MAXNUM = 512;
}

BulletGO::BulletGO(const CVector3& pos, const CVector3& move, IFu* owner, float damege, bool isLockable)
	: m_vector(move), m_owner(owner), ILifeObject(isLockable, owner)
{
	//�������W
	SetPos(pos), m_posOld = GetPos();
	m_defaultPos = GetPos();
	
	//�U������
	//�Փˏ���
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			for (auto& component : m_components) {
				if (!component->GetEnable()) { continue; }
				component->Contact(p);
			}		
		}
	);
	//�ՓˑO����
	m_col.m_reference.m_preCollisionFunc = 
		[&](ReferenceCollision* refcol) {
			bool isContact = true;
			for (auto& component : m_components) {
				if (!component->GetEnable()) { continue; }
				if (component->PreContact(refcol) == false) {
					isContact = false;
				}
			}
			return isContact;
	};
	m_col.m_collision.SetIsCollisionStaticObject(true);//�ÓI�I�u�W�F�N�g�Ƃ��Փ˂���
	m_col.m_collision.SetIsHighSpeed(true);//����͍����ł�

	m_col.m_reference.damege = damege;
	m_defaultDamege = damege;

	//TODO
	m_col.m_reference.attributes.set(enPhysical);
	m_col.m_reference.attributes.set(enFlame);

	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->Start();
	}
}

void BulletGO::Update() {
	//m_isMoved = true;

	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->Update();
	}

	//�ړ�
	m_posOld = GetPos();
	Move(m_vector);

	//���˓_����̍����Ђ炭(��������)�قǈЗ͏㏸
	if (m_isDamegeHeightBoost) {
		float power = max(0.0f, m_defaultPos.y - GetPos().y) / (LIMIT_HEIGHT_METER * METER);
		m_col.m_reference.damege = m_defaultDamege * (1.0f + power);

		//�G�t�F�N�g
		if (!m_damegeBoostEffect) {
			m_damegeBoostEffect = std::make_unique<CBillboard>();
			m_damegeBoostEffect->Init(L"Resource/effect/light.png", BOOST_EFECT_MAXNUM);
			m_damegeBoostEffect->SetRot({ CVector3::AxisZ(), CMath::RandomZeroToOne()*CMath::PI2 });
			m_damegeBoostEffect->GetModel().InitPostDraw(PostDrawModelRender::enAlpha);//�|�X�g�h���[
			m_damegeBoostEffect->GetModel().SetIsShadowCaster(false);
		}
		m_damegeBoostEffect->SetPos(GetPos());
		m_damegeBoostEffect->SetScale(3000.0f*power);
	}

	//��������
	m_lifeTime -= 1.0f;// FRAME_PER_SECOND;
	if (m_lifeTime < FLT_EPSILON) {
		new CShockRing(GetPos(), CVector4::White(), 400.0f);
		Death(); return;
	}

	//�R���W�����X�V
	m_col.SetPos(GetPos());
	m_col.SetVelocity(m_vector);

	//�x���V�e�B�ύX
	CalcVelocityUpdate(m_vector, m_gravity, m_upBrake, m_downAccel);

	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->PostUpdate();
	}
}

void BulletGO::CalcVelocityUpdate(CVector3& velocity, float gravity, float upBrake, float downAccel) {
	//�d��
	velocity.y -= gravity;

	if (velocity.y > 0.0f) {
		//�㏸����
		velocity = velocity.GetNorm() * (velocity.Length() + min(-velocity.y*upBrake, 0.0f));
	}
	else {
		//��������
		velocity = velocity.GetNorm() * (velocity.Length() + max(-velocity.y*downAccel, 0.0f));
	}
}

void BulletGO::PreLoopUpdate() {
	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->PreLoopUpdate();
	}
}

void BulletGO::PostLoopUpdate() {
	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->PostLoopUpdate();
	}
}

void BulletGO::Pre3DRender(int n) {
	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->Pre3DRender(n);
	}
}

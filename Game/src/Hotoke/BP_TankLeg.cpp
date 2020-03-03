#include "stdafx.h"
#include "BP_TankLeg.h"
#include "BulletHeader.h"

using namespace GameObj;

namespace {
	const CVector3 hitboxOffset = CVector3::Down()*50.f;
}

void BP_TankLeg::InnerStart() {
	m_name = L"�^���N";
	
	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/tankleg.cmo");
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
		if (me->EqualMaterialName(L"08 - Default")) {
			//����
			me->SetMetallic(0.5f);
			me->SetShininess(0.2f);
		}
		else {
			//�{��
			me->SetMetallic(0.75f);
			me->SetShininess(0.6f);
		}
	}
	);

	//���_�ʒu�{�[�����擾
	m_eye = m_model->FindBone(L"eye");

	//�r�{�[���擾
	m_legs[0][0] = m_model->FindBone(L"leg0s");
	m_legs[0][1] = m_model->FindBone(L"leg0e");
	m_legs[1][0] = m_model->FindBone(L"leg1s");
	m_legs[1][1] = m_model->FindBone(L"leg1e");
	m_legs[2][0] = m_model->FindBone(L"leg2s");
	m_legs[2][1] = m_model->FindBone(L"leg2e");

	m_legs[3][0] = m_model->FindBone(L"leg0s(mirrored)");
	m_legs[3][1] = m_model->FindBone(L"leg0e(mirrored)");
	m_legs[4][0] = m_model->FindBone(L"leg1s(mirrored)");
	m_legs[4][1] = m_model->FindBone(L"leg1e(mirrored)");
	m_legs[5][0] = m_model->FindBone(L"leg2s(mirrored)");
	m_legs[5][1] = m_model->FindBone(L"leg2e(mirrored)");

	//�����蔻��(��)
	//TODO
	{
		constexpr float radius = 50.0f;
		constexpr float height = 50.0f;
		const float modelScale = m_ptrCore->GetScale().GetMax() / (0.0188f*2.0f);
		//�`�󏉊���
		//m_col[lr].m_collision.CreateCapsule({}, {}, radius * modelScale, height*modelScale);
		m_col.m_collision.CreateSphere({}, {}, radius * modelScale);

		//ID�ݒ�
		m_col.m_reference.ownerID = m_ptrCore->GetFuID();
		m_col.m_reference.nonHitID = m_ptrCore->GetFuID();

		//�ʒu
		m_col.SetPos(hitboxOffset);
		m_beforePos = CVector3::Zero();

		//��������
		m_col.m_reference.attributes.set(enPhysical);

		//�Փˏ���
		m_col.m_reference.m_preCollisionFunc = [&](ReferenceCollision* H) {
			//�̓�����
			auto[damege, stunSec] = DHUtil::CalcRamDamege(m_col.m_reference.velocity, H->velocity);
			m_col.m_reference.damege = damege;
			m_col.m_reference.stunTimeSec = stunSec;
			return true;
		};
	}

	//�R���g���[���[
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_TankLeg(this, m_ptrCore);
	}
}

void BP_TankLeg::Update() {
	//�R���g���[���[�ɑ��삳����
	if (m_controller)m_controller->Update();

	//�ڒn����
	m_legHitNum = 0;
	float height = 0.0f;
	for (int i = 0; i < 6; i++) {
		//���C�Ŕ���
		btVector3 rayStart = m_legs[i][0]->GetPosition();
		btVector3 rayEnd = m_legs[i][1]->GetPosition();
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			m_legHitNum++;
			if (height < gnd_ray.m_hitPointWorld.y() - rayEnd.y()) {
				height = gnd_ray.m_hitPointWorld.y() - rayEnd.y();
			}
		}
	}
	if (m_legHitNum > 0) {
		//�{�̈ʒu�����
		m_ptrCore->SetPos(m_ptrCore->GetPos() + CVector3(0.0f, height, 0.0f));
		//m_ptrCore->AddVelocity(CVector3(0.0f, m_ptrCore->GetGravity(), 0.0f));
	}
}

void BP_TankLeg::PostUTRSUpdate() {
	//����̍X�V
	m_col.SetPos(m_ptrCore->GetPos() + hitboxOffset);
	m_col.SetVelocity(m_ptrCore->GetPos() - m_beforePos);
	m_beforePos = m_ptrCore->GetPos();

	//�Œ�J�����ݒ�
	m_ptrCore->SetUseFixedCamera(true);
	m_ptrCore->SetFixedCameraPoint(m_eye->GetPosition());

	//�ڒn����
	if (m_legHitNum > 0) {
		//�ڒn���Ă���Ȃ��RUP
		m_ptrCore->MulDrag(20.0f*m_legHitNum + 10.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));
		m_ptrCore->MulRotatability(3.0f*m_legHitNum + 1.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));//��]�͂�UP
		//�U��
		m_ptrCore->SetShakePower(0.0015f*max(0.0f, -m_ptrCore->GetTotalVelocity().y - m_ptrCore->GetGravity()));//*(hitnum/6.0f)
	}
}

void BP_TankLeg::Draw2D() {
}

void HCon_TankLeg::InnerUpdate() {
}
#include "stdafx.h"
#include "BP_KaniArm.h"
#include "BulletKani.h"

using namespace GameObj;

void BP_KaniArm::InnerStart() {
	//�A�j���[�V����(�{�[���������p)
	m_initPose.Load(L"Resource/animation/kaniarm.tka", true);

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/kaniarm.cmo", &m_initPose, 1);

	//�r��IK�ݒ�
	m_ikSetting[R] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[R]->tipBone = m_model->FindBone(L"Bone031");
	m_ikSetting[R]->rootBone = m_model->FindBone(L"Bone026");
	m_ikSetting[R]->iteration = 3;
	//m_ikSetting[R]->footIKRayEndOffset = 

	m_ikSetting[L] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[L]->tipBone = m_model->FindBone(L"Bone031(mirrored)");
	m_ikSetting[L]->rootBone = m_model->FindBone(L"Bone026(mirrored)");
	m_ikSetting[L]->iteration = 3;

	//
	m_muzzleBoneID[R] = m_model->FindBoneID(L"Bone031");
	m_muzzleBoneID[L] = m_model->FindBoneID(L"Bone031(mirrored)");

	//�G�t�F�N�g
	m_muzzleFlash.Init(L"Resource/spriteData/kaniFlare.png", 1, false);
	m_muzzleFlash.GetModel().InitPostDraw(PostDrawModelRender::enAdd);
	m_muzzleFlash.GetModel().GetSkinModel().SetCullMode(D3D11_CULL_NONE);
	m_muzzleFlash.GetModel().SetIsShadowCaster(false);

	//�R���g���[���[
	m_controller = new HCon_KaniArm(this,m_ptrCore);
}

void BP_KaniArm::Update() {
	//���t���[���̏�����
	m_isCharging = false;
	m_muzzleFlash.SetIsDraw(false);

	//�R���g���[���[�ɑ��삳����
	m_controller->Update();

	//�`���[�W���ԃ��Z�b�g
	if (!m_isCharging) { m_chargeTime = 0.0f; }
}

void BP_KaniArm::PostUTRSUpdate() {
	m_ikSetting[0]->targetPos = CVector3::Zero();
	m_ikSetting[1]->targetPos = CVector3::Zero();

	//�}�Y���G�t�F�N�g
	if (m_isCharging) {
		if (m_muzzleCnt <= 4) {
			m_muzzleFlash.SetIsDraw(true);
			m_muzzleFlash.SetPos(m_model->GetBonePos(m_muzzleBoneID[L]));
			m_muzzleFlash.SetScaleHoldAspectRatio(100.0f);
		}
		if (m_muzzleCnt > 8) { m_muzzleCnt = 0; }
	}

	//m_muzzleCnt++;
	//if (m_muzzleCnt <= 4) {
	//	m_muzzleFlash.SetIsDraw(true);
	//	m_muzzleFlash.SetPos(m_model->GetBonePos(m_muzzleBoneID[L]));
	//	m_muzzleFlash.SetScaleHoldAspectRatio(100.0f);

	//	std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>();
	//	billboard->Init(L"Resource/spriteData/smoke.png");
	//	//billboard->GetModel().InitPostDraw(PostDrawModelRender::enAlpha);
	//	billboard->SetPos(m_model->GetBonePos(m_muzzleBoneID[L]));
	//	billboard->SetScale(20.0f);
	//	SuicideObj::CParticle<CBillboard>* particle = new SuicideObj::CParticle<CBillboard>(std::move(billboard),30);
	//	particle->SetMove(CVector3(-1.0f,0.0f,-2.0f)*10.0f);
	//	particle->SetScaling(1.05f);

	//	new BulletKani(m_model->GetBonePos(m_muzzleBoneID[L]), (m_ikSetting[L]->targetPos - m_model->GetBonePos(m_muzzleBoneID[L])).GetNorm()*10.0f);
	//}
	//else {
	//	if (m_muzzleCnt >= 8) { m_muzzleCnt = 0; }
	//}
}

void BP_KaniArm::Charge() {
	//�`���[�W
	m_isCharging = true;//����Ń��Z�b�g
	m_chargeTime += GetDeltaTimeSec();
	if (m_chargeTime > m_machineGunTime) {
		//�}�V���K��
	}

	//�}�Y���G�t�F�N�g
	m_muzzleCnt++;
}
void BP_KaniArm::MachineGun() {

}
void BP_KaniArm::Rocket() {
	//if (m_chargeTime > FLT_EPSILON && m_chargeTime < m_fullCharge) {
		//�_�u���^�b�v
	//}
}
void BP_KaniArm::Lazer() {
	//���[�U�[�o��
}
void BP_KaniArm::Stab() {
	//�X�^�u�o��
}

//�q���[�}���R���g���[���[
void HCon_KaniArm::Update() {
	for (auto lr : { L, R }) {
		if (m_ptrCore->GetPad()->GetFire(lr)) {
			m_ptrBody->Charge();
		}
	}
}
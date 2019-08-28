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
	//�m�[�}���}�b�v�K�p
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	HRESULT hr = DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/Crab1NormalsMap.png", nullptr, textureView.ReleaseAndGetAddressOf());
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.7f);
		}
	);

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
	for (auto& model : m_muzzleFlash) {
		model.Init(L"Resource/spriteData/kaniFlare.png", 1, false);
		model.GetModel().InitPostDraw(PostDrawModelRender::enAdd);
		model.GetModel().GetSkinModel().SetCullMode(D3D11_CULL_NONE);
		model.GetModel().SetIsShadowCaster(false);
	}

	//�R���g���[���[
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_KaniArm(this, m_ptrCore);
	}
	else {
		m_controller = new AICon_KaniArm(this, m_ptrCore);
	}
}

void BP_KaniArm::Update() {
	//���t���[���̏�����
	for (int i = 0; i < enLRNUM; i++){
		m_isCharging[i] = false;
		m_isMachineGunning[i] = false;
		m_muzzleFlash[i].SetIsDraw(false);
	}	

	//�R���g���[���[�ɑ��삳����
	m_controller->Update();

	//�`���[�W���ԃ��Z�b�g
	for (int i = 0; i < enLRNUM; i++) {
		if (!m_isCharging[i]) { m_chargeTime[i] = 0; }
	}
}

void BP_KaniArm::PostUTRSUpdate() {
	for (int i = 0; i < enLRNUM; i++) {
		//IK�̖ڕW
		m_ikSetting[i]->targetPos = m_ptrCore->GetTargetPos();

		//�`���[�W
		if (m_isCharging[i] && !m_isMachineGunning[i]) {
			//�r���{�[�h�ǂݍ���
			std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>();
			billboard->Init(L"Resource/spriteData/smoke.png");
			billboard->GetModel().InitPostDraw(PostDrawModelRender::enAlpha);
			billboard->SetPos(m_model->GetBonePos(m_muzzleBoneID[i]));
			billboard->SetScale(20.0f);
			//�p�[�e�B�N����
			SuicideObj::CParticle<CBillboard>* particle = new SuicideObj::CParticle<CBillboard>(std::move(billboard), MACHINE_GUN_CHARGE_TIME);
			CVector3 move = CVector3::AxisX()*-40.0f; m_model->GetBoneRot(m_muzzleBoneID[i]).Multiply(move);
			particle->SetMove(move);
			particle->SetScaling(1.2f);
		}

		//�}�V���K��
		if (m_isMachineGunning[i]) {
			if (m_chargeTime[i]%5 == 0) {				
				//�}�Y���G�t�F�N�g
				m_muzzleTime[i] = 2;
				//����
				new BulletGO(
					nullptr,
					m_model->GetBonePos(m_muzzleBoneID[i]),
					(m_ikSetting[i]->targetPos - m_model->GetBonePos(m_muzzleBoneID[i])).GetNorm()*(100.0f+m_ptrCore->GetMove().Length())
				);
			}
		}

		//�}�Y���G�t�F�N�g
		if (m_muzzleTime[i] > 0) {
			m_muzzleTime[i] --;
			m_muzzleFlash[i].SetIsDraw(true);
			m_muzzleFlash[i].SetPos(m_model->GetBonePos(m_muzzleBoneID[i]));
			m_muzzleFlash[i].SetRot(m_model->GetBoneRot(m_muzzleBoneID[i])*CQuaternion(CVector3::AxisY(), CMath::PI_HALF));
			m_muzzleFlash[i].SetScaleHoldAspectRatio(100.0f);
		}
	}
}

void BP_KaniArm::ChargeAndMachinegun(enLR lr) {
	//�`���[�W
	m_isCharging[lr] = true;
	m_chargeTime[lr] ++;
	//�}�V���K��
	if (m_chargeTime[lr] > MACHINE_GUN_CHARGE_TIME) {
		m_isMachineGunning[lr] = true;
	}
}
void BP_KaniArm::Rocket(enLR lr) {
	//���P�b�g�o��

}
void BP_KaniArm::Lazer(enLR lr) {
	//���[�U�[�o��
	if (m_chargeTime[lr] >= LAZER_CHARGE_TIME && m_chargeTime[lr] < MACHINE_GUN_CHARGE_TIME) {

	}
}
void BP_KaniArm::Stab() {
	//�X�^�u�o��
}

//�q���[�}���R���g���[���[
void HCon_KaniArm::Update() {
	{
		m_ptrBody->Stab();
	}
	for (auto lr : { L, R }) {
		if (m_ptrCore->GetPad()->GetFire(lr)) {
			m_ptrBody->ChargeAndMachinegun(lr);
		}
		else {
			m_ptrBody->Lazer(lr);
		}
		if (m_ptrCore->GetPad()->GetDoubleTapFire(lr)) {
			m_ptrBody->Rocket(lr);
		}
	}
}

//AI�R���g���[���[
void AICon_KaniArm::Update() {
	for (auto lr : { L, R }) {
	//	m_ptrBody->ChargeAndMachinegun(lr);
	}
}
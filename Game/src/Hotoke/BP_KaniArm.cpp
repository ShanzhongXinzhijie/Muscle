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

	m_ikSetting[L] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[L]->tipBone = m_model->FindBone(L"Bone031(mirrored)");
	m_ikSetting[L]->rootBone = m_model->FindBone(L"Bone026(mirrored)");
	m_ikSetting[L]->iteration = 3;

	m_ikTargetPos[L] = CVector3::Down()*GetMainCamera()->GetFar();//TODO ���C���J�����g��?
	m_ikTargetPos[R] = m_ikTargetPos[L];

	//���ˌ��{�[��
	m_muzzleBoneID[R] = m_model->FindBoneID(L"Bone031");
	m_muzzleBoneID[L] = m_model->FindBoneID(L"Bone031(mirrored)");

	//�}�Y���t���b�V��
	for (auto& model : m_muzzleFlash) {
		model.Init(L"Resource/spriteData/kaniFlare.png", 1, false);
		//model.GetModel().InitPostDraw(PostDrawModelRender::enAdd);
		model.GetModel().GetSkinModel().SetCullMode(D3D11_CULL_NONE);//�o�b�N�J�����O���Ȃ�
		model.GetModel().SetIsShadowCaster(false);//�e���Ƃ��Ȃ�
		//�}�e���A���ݒ�	
		model.GetModel().GetSkinModel().FindMaterialSetting(
			[&](MaterialSetting* mat) {
				//mat->SetAlbedoScale({ 0.0f,0.0f,1.0f,1.0f });//�F�ύX
				mat->SetEmissive(16.0f*0.5f);//����
				mat->SetLightingEnable(false);//���C�e�B���O���Ȃ�
				//mat->SetIsMotionBlur(false);//���[�V�����u���[�����Ȃ�
			}
		);
	}

	//�R���g���[���[
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_KaniArm(this, m_ptrCore);
	}
	else {
		m_controller = new AICon_KaniArm(this, m_ptrCore);
	}

	//HUD
	m_guncross.Init(L"Resource/spriteData/kaniMark.png");
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
		//IK�^�[�Q�b�g�̈ړ�

		//TODO �r�̉�]�����Ƃɂ��?

		CVector3 targetDir = m_ptrCore->GetTargetPos() - m_model->GetBonePos(m_muzzleBoneID[i]); targetDir.Normalize();

		//Z����������֌�����N�H�[�^�j�I�����쐬
		CQuaternion LorRrot; LorRrot.MakeLookToUseXYAxis(i != L ? m_ptrCore->GetLeft() : m_ptrCore->GetRight());
		//������̃��[�J���^�[�Q�b�g�������Z�o
		CVector3 zLocalTargetDir = targetDir; LorRrot.InverseMultiply(zLocalTargetDir);
		CVector3 zLocalTargetDirNorm = zLocalTargetDir; zLocalTargetDirNorm.Normalize();
		//Z�������[�J���^�[�Q�b�g�����֌�����N�H�[�^�j�I�����쐬
		CQuaternion toTargetRot;
		toTargetRot.MakeLookToUseXYAxis(zLocalTargetDir);
		//��]���Ɗp�x���擾
		CVector3 axis; float angle;
		axis = CVector3::AxisZ();
		axis.Cross(zLocalTargetDir); axis.Normalize();
		angle = CVector3::AngleOf2NormalizeVector(zLocalTargetDirNorm, CVector3::AxisZ());
		//toTargetRot.ToAngleAxis(axis, angle);		
		//�p�x����
		if (m_ptrCore->GetFront().Dot(targetDir) > 0.0f) {
			angle = min(angle, CMath::DegToRad(110.0f));//�O������
		}
		else {
			angle = min(angle, CMath::DegToRad(10.0f));//�������
		}

		//���݂̘r�̊p�x
		//CVector3 zLocalArmDir = m_ptrCore->GetPos() + m_ptrCore->GetRot().GetMultiply(m_ikTargetPos[i]);//��[��ǂɂ���
		//LorRrot.InverseMultiply(zLocalArmDir);//���[�J���ɕϊ�
		//float armAngle = CVector3::AngleOf2NormalizeVector(zLocalArmDir.GetNorm(), CVector3::AxisZ());
		//if (abs(armAngle - angle) < CMath::DegToRad(1.0f)) {
		//	armAngle = angle;
		//}
		//else {
		//	armAngle += std::copysign(CMath::DegToRad(1.0f),angle - armAngle);
		//}
		//m_angle[0] = angle;
		//m_angle[1] = armAngle;

		toTargetRot.SetRotation(axis, angle);// armAngle);

		//���[���h��Ԃ̃^�[�Q�b�g�������Z�o
		CVector3 outDir = CVector3::AxisZ();
		toTargetRot.Multiply(outDir); 
		LorRrot.Multiply(outDir);
		//m_ikTargetPos[i] = outDir;

			//���[���h���W�n�ɂ�����^�[�Q�b�g���W���߂�
			CVector3 worldTargetPos = m_ikTargetPos[i];
			m_ptrCore->GetRot().Multiply(worldTargetPos);
			worldTargetPos += m_ptrCore->GetPos();

			constexpr float armSpeed = 0.2f;			
			CVector3 armDir = worldTargetPos - m_model->GetBonePos(m_muzzleBoneID[i]); armDir.Normalize();
			
			if ((outDir - armDir).LengthSq() < CMath::Square(armSpeed)) {
				m_ikTargetPos[i] = outDir;
			}
			else {
				m_ikTargetPos[i] = armDir + (outDir - armDir).GetNorm()*armSpeed;
			}

		m_ikTargetPos[i] *= (m_ptrCore->GetTargetPos() - m_model->GetBonePos(m_muzzleBoneID[i])).Length();
		m_ikTargetPos[i] += m_model->GetBonePos(m_muzzleBoneID[i]);

		//�^�[�Q�b�g���W�����[�J�����W�^�ɕϊ�
		m_ikTargetPos[i] -= m_ptrCore->GetPos();
		CVector3 localTargetPos;
		localTargetPos.x = m_ptrCore->GetRight().Dot(m_ikTargetPos[i]);
		localTargetPos.y = m_ptrCore->GetUp().Dot(m_ikTargetPos[i]);
		localTargetPos.z = m_ptrCore->GetFront().Dot(m_ikTargetPos[i]);
		m_ikTargetPos[i] = localTargetPos;

		//IK�̃^�[�Q�b�g
		m_ikSetting[i]->targetPos = m_ptrCore->GetPos() + m_ptrCore->GetRot().GetMultiply(m_ikTargetPos[i]);

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
				CVector3 dirNorm = (m_ikSetting[i]->targetPos - m_model->GetBonePos(m_muzzleBoneID[i])).GetNorm();
				BulletGO* bullet = new BulletGO(
					m_model->GetBonePos(m_muzzleBoneID[i]),
					(dirNorm*100.0f)+m_ptrCore->GetVelocity()
				);
				bullet->AddComponent(std::make_unique<BD_BeamModel>(3.0f,L"BLUE"));
				bullet->AddComponent(std::make_unique<BD_Contact>());
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

void BP_KaniArm::Draw2D() {
	if (!m_ptrCore->GetIsDrawHUD()) { return; }

	//�r�̏Ə�
	for (auto lr : { L, R }) {
		CVector3 pos = GetMainCamera()->CalcScreenPosFromWorldPos(m_ikSetting[lr]->targetPos);//TODO ���C���J�����g��?
		if (pos.z > 0.0f && pos.z < 1.0f) {
			m_guncross.Draw(pos, 0.5f, { lr == R ? 0.0f : 1.0f,0.5f }, 0.0f, m_ptrCore->GetFont()->GetColor(), lr == L ? DirectX::SpriteEffects::SpriteEffects_None : DirectX::SpriteEffects::SpriteEffects_FlipHorizontally);
		}
	}

	//debug
	//m_ptrCore->GetFont()->DrawFormat(L"L:%.3f\nR:%.3f", { 0.0f,0.6f }, {}, m_angle[L], m_angle[R]);
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
	//�}�Y���G�t�F�N�g
	m_muzzleTime[lr] = 2;
	//����
	CVector3 dirNorm = (m_ikSetting[lr]->targetPos - m_model->GetBonePos(m_muzzleBoneID[lr])).GetNorm();
	BulletGO* bullet = new BulletGO(
		m_model->GetBonePos(m_muzzleBoneID[lr]),
		(dirNorm*100.0f) + m_ptrCore->GetVelocity()
	);
	bullet->AddComponent(std::make_unique<BD_BeamModel>(30.0f, L"Red"));
	bullet->AddComponent(std::make_unique<BD_Contact>());
	bullet->AddComponent(std::make_unique<BD_Homing>(m_ptrCore->GetTarget(), 10.0f, 0.0f, 30.0f));
	bullet->AddComponent(std::make_unique<BD_Brake>(1.0f));
}
void BP_KaniArm::Lazer(enLR lr) {
	//�O���l�[�h�o��
	if (m_chargeTime[lr] >= LAZER_CHARGE_TIME && m_chargeTime[lr] < MACHINE_GUN_CHARGE_TIME) {
		//�}�Y���G�t�F�N�g
		m_muzzleTime[lr] = 2;
		//����
		CVector3 dirNorm = (m_ikSetting[lr]->targetPos - m_model->GetBonePos(m_muzzleBoneID[lr])).GetNorm();
		BulletGO* bullet = new BulletGO(
			m_model->GetBonePos(m_muzzleBoneID[lr]),
			dirNorm*(100.0f + dirNorm.Dot(m_ptrCore->GetVelocity()))
		);
		bullet->AddComponent(std::make_unique<BD_BeamModel>(3.0f, L"Yellow"));
		bullet->AddComponent(std::make_unique<BD_ContactExplosion>());
	}
}
void BP_KaniArm::Stab() {
	//�X�^�u�o��
}

//�q���[�}���R���g���[���[
void HCon_KaniArm::Update() {
	/*{
		m_ptrBody->Stab();
	}*/
	for (auto lr : { L, R }) {
		if (m_ptrCore->GetPad()->GetFire(lr)) {
			m_ptrBody->ChargeAndMachinegun(lr);
		}
		/*else {
			m_ptrBody->Lazer(lr);
		}*/
		if (m_ptrCore->GetPad()->GetDoubleTapFire(lr)) {
			m_ptrBody->Rocket(lr);
		}
	}
}

//AI�R���g���[���[
void AICon_KaniArm::Update() {
	if (m_ptrCore->GetAIStatus()->isAttackingTarget) {
		for (auto lr : { L, R }) {
			m_ptrBody->ChargeAndMachinegun(lr);
		}
	}
}
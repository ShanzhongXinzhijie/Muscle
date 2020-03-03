#include "stdafx.h"
#include "BP_DinoArm.h"
#include "BulletHeader.h"
#include "CSmoke.h"

using namespace GameObj;

namespace {
	std::wstring string;
}

void BP_DinoArm::InnerStart() {
	m_name = L"�_�C�i�\�[�Y";

	//�A�j���[�V����(�{�[���������p)
	m_initPose.Load(L"Resource/animation/DinosaurArms.tka", true);

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/DinosaurArms.cmo", &m_initPose, 1);
	////�m�[�}���}�b�v�K�p
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	//HRESULT hr = DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/Crab1NormalsMap.png", nullptr, textureView.ReleaseAndGetAddressOf());
	//m_model->GetSkinModel().FindMaterialSetting(
	//	[&](MaterialSetting* mat) {
	//	mat->SetNormalTexture(textureView.Get());
	//	mat->SetShininess(0.7f);
	//}
	//);

	//�r��IK�ݒ�
	m_ikSetting[R] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[R]->tipBone = m_model->FindBone(L"Bone021");
	m_ikSetting[R]->rootBone = m_model->FindBone(L"Bone014");
	//m_ikSetting[R]->iteration = 3;

	m_ikSetting[L] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[L]->tipBone = m_model->FindBone(L"Bone021(mirrored)");
	m_ikSetting[L]->rootBone = m_model->FindBone(L"Bone014(mirrored)");
	//m_ikSetting[L]->iteration = 3;

	m_ikTargetPos[L] = CVector3::Down()*GetMainCamera()->GetFar();
	m_ikTargetPos[R] = m_ikTargetPos[L];

	//���ˌ��{�[��
	m_muzzleBoneID[R] = m_model->FindBoneID(L"Bone021");
	m_muzzleBoneID[L] = m_model->FindBoneID(L"Bone021(mirrored)");

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
		m_controller = new HCon_DinoArm(this, m_ptrCore);
	}
	else {
		m_controller = new AICon_DinoArm(this, m_ptrCore);
	}

	//HUD
	m_guncross.Init(L"Resource/spriteData/kaniMark.png");
}

void BP_DinoArm::Update() {
	//���t���[���̏�����
	for (int i = 0; i < enLRNUM; i++) {
		m_isCharging[i] = false;
		m_isMachineGunning[i] = false;
		m_muzzleFlash[i].SetIsDraw(false);
		m_coolDown[i] = max(0, m_coolDown[i] - 1);//�N�[���_�E��
	}

	//�R���g���[���[�ɑ��삳����
	m_controller->Update();

	for (int i = 0; i < enLRNUM; i++) {
		if (!m_isCharging[i]) {
			//�`���[�W���ԃ��Z�b�g
			m_chargeTime[i] = 0;
		}
		else {
			//�`���[�W���Ԃ��炷(�������˂͂��[��)
			if (m_chargeTime[L] == m_chargeTime[R]) {
				m_chargeTime[i]++;
			}
		}
	}
}

void BP_DinoArm::PostUTRSUpdate() {
	for (int i = 0; i < enLRNUM; i++) {
		//�r�̖ڕW�_(IK�^�[�Q�b�g)�̍X�V
		m_ikSetting[i]->targetPos = TurnArm(m_isMachineGunning[i] ? m_ptrCore->GetTargetPos() : m_ptrCore->GetVanisingPoint(), m_model->GetBonePos(m_muzzleBoneID[i]), i != L ? m_ptrCore->GetLeft() : m_ptrCore->GetRight(), 110.0f, 110.0f, 0.2f, m_ikTargetPos[i]);

		//�`���[�W
		if (m_isCharging[i] && !m_isMachineGunning[i]) {
			CVector3 offset = CVector3::AxisX()*-40.0f;
			m_model->GetBoneRot(m_muzzleBoneID[i]).Multiply(offset);

			//�r���{�[�h�ǂݍ���
			std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>();
			billboard->Init(L"Resource/spriteData/smoke.png");
			billboard->GetModel().InitPostDraw(PostDrawModelRender::enAlpha);
			billboard->SetPos(
				m_model->GetBonePos(m_muzzleBoneID[i])
				+ CVector3(CMath::RandomZeroToOne(), CMath::RandomZeroToOne(), CMath::RandomZeroToOne())*5.0f
				+ offset
			);
			billboard->SetRot({ CVector3::AxisZ(),CMath::PI2*CMath::RandomZeroToOne() });
			billboard->SetScale(20.0f);
			//�p�[�e�B�N����
			SuicideObj::CParticle<CBillboard>* particle = new SuicideObj::CParticle<CBillboard>(std::move(billboard), MACHINE_GUN_CHARGE_TIME);
			CVector3 move = offset + m_ptrCore->GetTotalVelocity();
			particle->SetMove(move);
			particle->SetRotation({ CVector3::AxisZ(), 0.1f + 0.1f * CMath::RandomZeroToOne() });
			particle->SetScaling(1.2f);

			/*CVector3 move = offset + m_ptrCore->GetTotalVelocity();
			new CSmoke(
				m_model->GetBonePos(m_muzzleBoneID[i])
				+ CVector3(CMath::RandomZeroToOne(), CMath::RandomZeroToOne(), CMath::RandomZeroToOne())*5.0f
				+ offset,
				move,
				{ 1.0f,1.0f,1.0f,1.0f },
				20.0f,
				2.0f
			);*/
		}

		//�}�V���K��
		if (m_isMachineGunning[i]) {
			if (m_chargeTime[i] % 5 == 0) {
				//�}�Y���G�t�F�N�g
				m_muzzleTime[i] = 2;
				//����
				constexpr float bulletSpeed = 100.0f*2.0f;//�e��

				float aimPow = 0.0f;
				if (m_ptrCore->GetTarget()) {
					aimPow = 1.0f;
				}

				//�΍��ˌ�
				float hitTime = 0.0f;
				CVector3 aimOffset;
				if (m_ptrCore->GetTarget() && aimPow > 0.0f) {
					CVector3 dirNorm = ((m_ikSetting[i]->targetPos + aimOffset) - m_model->GetBonePos(m_muzzleBoneID[i])).GetNorm();
					CVector3 kansei = m_ptrCore->GetTotalVelocity();
					float plusspeed = dirNorm.Dot(kansei);
					float totalSpeed = bulletSpeed + plusspeed;

					CVector3 OL = m_ptrCore->GetTargetPos() - m_model->GetBonePos(m_muzzleBoneID[i]);
					CVector3 v = m_ptrCore->GetTarget()->GetMove();
					float sita = CVector3::AngleOf2NormalizeVector(OL.GetNorm(), v.GetNorm());
					float vLength = v.Length();
					float toTargetDistance = OL.Length();

					/*CVector3 vector = dirNorm * totalSpeed;
					for (int i = 0; i < toTargetDistance / totalSpeed; i++) {
						BulletGO::CalcVelocityUpdate(vector);
					}
					totalSpeed = (totalSpeed+vector.Length())/2.0f;*/

					float t1 = toTargetDistance / (totalSpeed * sqrt(1.0f - CMath::Square(vLength / totalSpeed * sin(sita))) - vLength * cos(sita));
					float t2 = toTargetDistance / (-totalSpeed * sqrt(1.0f - CMath::Square(vLength / totalSpeed * sin(sita))) - vLength * cos(sita));

					t1 = max(t1, t2);
					if (t1 > 0.0f) {
						aimOffset = v * t1;
						hitTime = t1;
					}
				}

				/*
				//�����̊������l�������΍�
				CVector3 vector = (m_ikSetting[i]->targetPos + aimOffset) - m_model->GetBonePos(m_muzzleBoneID[i]);
				if (vector.LengthSq() > FLT_EPSILON) {
					float distance = vector.Length();
					vector.Normalize();
					CVector3 toTarget = vector;
					vector *= bulletSpeed;
					vector += m_ptrCore->GetTotalVelocity();

					//float cnt2 = distance / (bulletSpeed+ toTarget.Dot(m_ptrCore->GetTotalVelocity()));

					int cnt = 0;
					while (true)
					{
						float moveLength = max(0.0f, toTarget.Dot(vector));
						distance -= moveLength;
						if (moveLength < 1.0f || distance < 1.0f) {
							break;
						}
						cnt++;
						BulletGO::CalcVelocityUpdate(vector, 0.0f);
					}

					aimOffset -= (m_ptrCore->GetTotalVelocity() - toTarget * toTarget.Dot(m_ptrCore->GetTotalVelocity()))*cnt;
				}
				*/

				aimOffset *= aimPow;

				CVector3 dirNorm = ((m_ikSetting[i]->targetPos + aimOffset) - m_model->GetBonePos(m_muzzleBoneID[i])).GetNorm();

				CVector3 kansei = m_ptrCore->GetTotalVelocity();
				kansei = dirNorm * dirNorm.Dot(kansei);
				kansei.Lerp(aimPow, m_ptrCore->GetTotalVelocity(), kansei);

				BulletGO* bullet = new BulletGO(
					m_model->GetBonePos(m_muzzleBoneID[i]),
					(dirNorm*bulletSpeed) + kansei,
					m_ptrCore,
					1.0f
				);
				bullet->AddComponent(std::make_unique<BD_BeamModel>(3.0f, L"BLUE"));
				//bullet->AddComponent(std::make_unique<BD_Tracking>(m_ptrCore->GetTarget()));
				//std::unique_ptr<BD_Timer> bd_timer = std::make_unique<BD_Timer>(hitTime/2.0f);
				//bd_timer->AddComponent(&bullet->GetComponentBack());
				//bullet->AddComponent(std::move(bd_timer));
				bullet->AddComponent(std::make_unique<BD_Reflect>());
				bullet->AddComponent(std::make_unique<BD_Contact>(false));
				//bullet->m_gravity = 0.2f;
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

void BP_DinoArm::Draw2D() {
	if (!m_ptrCore->GetIsDrawHUD()) { return; }

	//�r�̏Ə�
	for (auto lr : LR) {
		CVector3 pos = GetMainCamera()->CalcScreenPosFromWorldPos(m_ikSetting[lr]->targetPos);//TODO ���C���J�����g��?
		if (pos.z > 0.0f && pos.z < 1.0f) {
			CVector4 color = m_coolDown[lr] > 0 ? m_ptrCore->GetWarningFont()->GetColor() : m_ptrCore->GetFont()->GetColor();
			m_guncross.Draw(pos, 0.5f, { lr == R ? 0.0f : 1.0f,0.5f }, 0.0f, color, lr == L ? DirectX::SpriteEffects::SpriteEffects_None : DirectX::SpriteEffects::SpriteEffects_FlipHorizontally);
		}
	}

	//����	
	//TODO ��������p�t�H���g
	//CVector2 beforeScale = m_ptrCore->GetFont()->GetScale();
	//m_ptrCore->GetFont()->SetScale(0.375f);
	//m_ptrCore->GetFont()->SetUseFont(HUDFont::enJPN);
	//m_ptrCore->GetFont()->Draw(L"[LB/RB](�Ȃ�����): �}�V���K��\n[LB/RB](2�ǂ���): �~�b�\�[", {0.0f,0.5f});
	///*if (m_coolDown[L] > 0) {
	//	m_ptrCore->GetFont()->Draw(L"///////////////////////\n///////////////////////////", { 0.0f,0.5f });
	//}*/
	//m_ptrCore->GetFont()->SetUseFont(HUDFont::enENG);
	//m_ptrCore->GetFont()->SetScale(beforeScale);

	//�N�[���_�E��
	for (auto lr : LR) {
		if (m_coolDown[lr] > 0) {
			int leftTime = m_coolDown[lr] / FRAME_RATE;
			string.clear();
			if (lr == L) {
				string += L"COOLDOWN";
				for (int i = 0; i < leftTime; i++) {
					string += L"<";
				}
				m_ptrCore->GetWarningFont()->Draw(string, { 0.0f,0.5f });
			}
			else {
				for (int i = 0; i < leftTime; i++) {
					string += L">";
				}
				string += L"COOLDOWN";
				m_ptrCore->GetWarningFont()->Draw(string, { 1.0f,0.5f }, { 1.1f,0.0f });
			}
		}
	}

	//debug
	//m_ptrCore->GetFont()->DrawFormat(L"L:%.3f\nR:%.3f", { 0.0f,0.6f }, {}, m_angle[L], m_angle[R]);
}

void BP_DinoArm::ChargeAndMachinegun(enLR lr) {
	if (m_coolDown[lr] <= 0) {
		//�`���[�W
		m_isCharging[lr] = true;
		m_chargeTime[lr] ++;
		//�}�V���K��
		if (m_chargeTime[lr] > MACHINE_GUN_CHARGE_TIME) {
			m_isMachineGunning[lr] = true;
		}
	}
}
void BP_DinoArm::Rocket(enLR lr) {
	//���P�b�g�o��
	if (m_coolDown[lr] <= 0) {
		//�}�Y���G�t�F�N�g
		m_muzzleTime[lr] = 2;
		//�N�[���_�E��
		m_coolDown[lr] = ROCKET_COOLDOWN;

		constexpr float homingStartTime = 50.0f*2.0f;//�z�[�~���O�J�n����
		constexpr float thrust = 10.0f;//����

		//����
		CVector3 dirNorm = (m_ikSetting[lr]->targetPos - m_model->GetBonePos(m_muzzleBoneID[lr])).GetNorm();
		//�e����
		BulletGO* bullet = new BulletGO(
			m_model->GetBonePos(m_muzzleBoneID[lr]),//�ʒu
			(dirNorm*60.0f) + m_ptrCore->GetTotalVelocity(),//�ړ�
			m_ptrCore,//�I�[�i�[
			10.0f//�_���[�W
		);

		//�X�e�[�^�X
		bullet->m_lifeTime = 230.0f;
		bullet->m_downAccel = 0.0f;
		bullet->m_upBrake = 0.0f;

		//�R���|�[�l���g

		//�z�[�~���O�J�n���ɃR���|�[�l���g���I���ɂ���^�C�}�[
		std::unique_ptr<BD_Timer> bd_OnTimer = std::make_unique<BD_Timer>(homingStartTime);
		//�I�t�ɂ���^�C�}�[
		std::unique_ptr<BD_Timer> bd_OffTimer = std::make_unique<BD_Timer>(homingStartTime, false);

		//���f��
		bullet->AddComponent(std::make_unique<BD_BeamModel>(30.0f, L"Red"));
		//���O��
		bullet->AddComponent(std::make_unique<BD_SmokeTrail>());
		bd_OnTimer->AddComponent(&bullet->GetComponentBack());//�I���^�C�}�[�ɐݒ�
		//�Փ�
		bullet->AddComponent(std::make_unique<BD_Contact>());
		//�Փˉ��
		bullet->AddComponent(std::make_unique<BD_ClashAvoidance>(thrust));
		bd_OffTimer->AddComponent(&bullet->GetComponentBack());//�I�t�^�C�}�[�ɐݒ�
		//�z�[�~���O
		std::unique_ptr<BD_Homing> homing = std::make_unique<BD_Homing>(m_ptrCore->GetTarget(), thrust, 0.0f, CMath::DegToRad(67.5f), homingStartTime);
		if (m_ptrCore->GetTarget()) {
			homing->SetInitDirection(m_ptrCore->GetTarget()->GetFu()->GetCollisionPos() - bullet->GetPos());
		}
		bullet->AddComponent(std::move(homing));
		//����
		bullet->AddComponent(std::make_unique<BD_Brake>(1.0f));
		//���b�N�I���\(�D��x+2)
		bullet->AddComponent(std::make_unique<BD_Lockable>(LockableWrapper::DEFAULT_LEVEL + 2));
		//�^�C�}�[��ݒ�
		bullet->AddComponent(std::move(bd_OffTimer));
		bullet->AddComponent(std::move(bd_OnTimer));
	}
}

//�q���[�}���R���g���[���[
void HCon_DinoArm::InnerUpdate() {
	for (auto lr : { L, R }) {
		if (m_ptrCore->GetPad()->GetFire(lr)) {
			m_ptrBody->ChargeAndMachinegun(lr);
		}
		if (m_ptrCore->GetPad()->GetDoubleTapFire(lr)) {
			m_ptrBody->Rocket(lr);
		}
	}
}
//AI�R���g���[���[
void AICon_DinoArm::InnerUpdate() {
	if (m_ptrCore->GetAIStatus()->isAttackingTarget) {
		for (auto lr : { L, R }) {
			m_ptrBody->ChargeAndMachinegun(lr);
			if (lr == L && m_cnt < 10 || lr == R && m_cnt > 10) {
				m_ptrBody->Rocket(lr);
			}
		}
	}
	m_cnt++; if (m_cnt > 20) { m_cnt = 0; }
}
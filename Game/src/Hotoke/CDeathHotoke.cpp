#include "stdafx.h"
#include "CDeathHotoke.h"
#include "TimeManager.h"

#include "CSmoke.h"
#include "CBlood.h"

namespace {
	constexpr float modelScale = 0.0188f*2.0f;//���f���̃X�P�[��

	CVector3 CalcCollisionPosOffset(const CVector3& scale) {
		return { 0.0f, 90.0f*(scale.y / (0.0188f*2.0f)), -15.0f*(scale.z / (0.0188f*2.0f)) };
	}
}

void CDeathHotoke::SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part) {
	m_parts[partsType] = std::move(part);
	m_parts[partsType]->Init(this);
	if (GetIsStart()) { m_parts[partsType]->Start(); }
}

bool CDeathHotoke::Start() {
	//�X�P�[��
	m_scale = { modelScale };

	//�����ʒu
	//���C�Ŕ���
	//btVector3 rayStart = GetPos();
	//btVector3 rayEnd = GetPos(); rayEnd.setY(-100000.0f);
	//btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
	//GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
	//if (gnd_ray.hasHit()) {
	//	//�ڐG�_�����W��
	//	SetPos(gnd_ray.m_hitPointWorld);
	//}

	//�R�A����
	m_coreModel.Init(L"Resource/modelData/core.cmo");	
	//�m�[�}���}�b�v�K�p
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;	
	TextureFactory::GetInstance().Load(L"Resource/normalMap/ngasi.png", nullptr, &textureView, nullptr, true);
	m_coreModel.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.9f);
			mat->SetTranslucent(0.2f);
		}
	);

	//���w
	m_kouhai.Init(L"Resource/effect/haikouCliping.png", 1, false);
	m_kouhai.GetModel().GetSkinModel().SetCullMode(D3D11_CULL_NONE);//�o�b�N�J�����O���Ȃ�
	//m_kouhai.GetModel().InitPostDraw(PostDrawModelRender::enAlpha, false, true);//�|�X�g�h���[(�\�t�g�p�[�e�B�N��)
	m_kouhai.GetModel().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetIsMotionBlur(false);
			mat->SetLightingEnable(false);
		}
	);

	//�����蔻��
	CreateCapsule({}, {}, 60.0f*(m_scale.x / (0.0188f*2.0f)), 30.0f*(m_scale.y / (0.0188f*2.0f)));
	
	//ID�ݒ�
	SetCollisionOwnerID(GetFuID());
	SetCollisionNonHitID(GetFuID());

	//�Փˎ��Ɏ��s���鏈��
	SetCollisionFunc(
		[&](ReferenceCollision* H, SuicideObj::CCollisionObj::SCallbackParam& p) {			
			//�_���[�W
			Damage(*H, p.m_collisionPoint);
			//�X�^��
			m_stunTimeSec = max(m_stunTimeSec,H->stunTimeSec);
			
			//�Y�[���A�E�g
			if (H->stunTimeSec > 0.0f) {
				//�X���[���[�V����
				constexpr int SLOWMO_FPS = 6;
				constexpr float SLOWMO_SEC = 1.4f;

				//�U���H������ق�(������)
				CVector3 dir;
				dir = H->velocity; dir.y = 0.0f; dir.Normalize();
				if (dir.LengthSq() < FLT_EPSILON) {
					dir = GetFront();
				}
				//CQuaternion(CVector3::AxisY(), CMath::PI_HALF / 2.0f).Multiply(dir);
				m_zoomoutDir = dir * -800.f;
				m_zoomoutDir.y += 400.0f;
				SetZoomoutTime(m_stunTimeSec);
				SetZoomoutTarget(this);				

				//�U�����Ă����̉��o
				QueryGOs<CDeathHotoke>(L"CDeathHotoke",
					[&](CDeathHotoke* go) {
						if (go == this) { return true; }
						go->SetZoomoutTime(SLOWMO_SEC * ((FRAME_PER_SECOND * SLOWMO_FPS)));//�X���[���[���I���܂�
						go->SetZoomoutDirection(m_zoomoutDir);
						go->SetZoomoutTarget(this);
						return true;
					}
				);

				//�X���[���[�V����
				TimeManager::GetInstance().SetFrameRate(SLOWMO_SEC, SLOWMO_FPS, 1.0f);
			}
		}
	);
	SetPreCollisionFunc(
		[&](ReferenceCollision* H) {
			//�̓�����
			auto[damege,stunSec] = DHUtil::CalcRamDamege(GetReferenceCollision().velocity, H->velocity);
			SetDamegePower(damege);//�_���[�W	
			SetStunSec(stunSec);//�X�^���b��			
			//TODO �̓�����̒��˕Ԃ� ���i�q�b�g
			return true;
		}
	);
	SetCollisionPosOffset(CalcCollisionPosOffset(m_scale));//�ʒu
	GetAttributes().set(enPhysical);//��������
	GetAttributes().set(enHotokeBody);//�z�g�P�̑̑���
	//m_col.m_collision.SetIsHighSpeed(true);//����͍����ł�	

	//�ʒu������	
	m_posOld = GetPos();

	//�p�[�c��Start
	for (auto& part : m_parts) {
		if (part)part->Start();
	}

	//SE
	GetWAVSettingManager().Load(L"Resource/sound/Hit_Hurt.wav")->volume = 0.5f;
	GetWAVSettingManager().Load(L"Resource/sound/Hit_Hurt2.wav")->volume = 0.5f;
	GetWAVSettingManager().Load(L"Resource/sound/Hit_Hurt5.wav")->volume = 0.5f;
	
	return true;
}

void CDeathHotoke::PreLoopUpdate() {
	//�`�揉����
	m_kouhai.SetIsDraw(m_isDrawKouhai);
}

void CDeathHotoke::PreUpdate() {
	//�����W�L�^
	m_posOld = GetPos();

	//�U����
	if (m_shakePower / 0.0015f > 1.0f) {
		float length = 150.0f;
		switch (CMath::RandomInt() % 3) {
		case 0:
			new GameSE(L"Resource/sound/Hit_Hurt.wav", GetPos(), length, GetPlayerNum());
			break;
		case 1:
			new GameSE(L"Resource/sound/Hit_Hurt2.wav", GetPos(), length, GetPlayerNum());
			break;
		case 2:
			new GameSE(L"Resource/sound/Hit_Hurt5.wav", GetPos(), length, GetPlayerNum());
			break;
		default:
			break;
		}
	}

	//�X�e�[�^�X�X�V
	m_drag[enNow] = m_drag[enNext];
	m_angularDrag[enNow] = m_angularDrag[enNext];
	m_rotatability[enNow] = m_rotatability[enNext];
	m_drag[enNext] = 1.0f;
	m_angularDrag[enNext] = 1.0f;
	m_rotatability[enNext] = 1.0f;
	m_shakePower = 0.0f;
	m_damegePower *= 0.5f;

	if (m_isInvincible) {
		//�\��������
		m_coreModel.GetSkinModel().FindMaterialSetting(
			[](MaterialSetting* mat) {
				mat->SetAlbedoScale(CVector4::White());
			}
		);
		//���菉����
		SetCollisionEnable(true);
		//���G�I��
		m_isInvincible = false;
	}
}

void CDeathHotoke::Update() {
	//AI���s
	if (m_ai) { m_ai->Update(); }

	//�X�^������
	Stun();
	//�Y�[���A�E�g����
	m_zoomoutTimeSec -= FRAME_PER_SECOND;
	if (m_zoomoutTimeSec < 0.0f) {
		m_zoomoutTimeSec = 0.0f;
	}

	//�ړ��K��
	Move(GetTotalVelocity());
	SetRot(m_angularVelocity * GetRot());
	//����
	m_veloxity *= 0.5f*(1.0f/m_drag[enNow]);
	float linearLength = m_linearVelocity.Length();
	if (linearLength > FLT_EPSILON) {
		m_linearVelocity *= max(0.0f, linearLength - (1.0f + m_drag[enNow]*0.1f)) / linearLength;
	}
	m_angularVelocity.Slerp(0.5f*(1.0f / m_drag[enNow])*(1.0f / m_angularDrag[enNow]), CQuaternion::Identity(), m_angularVelocity);
	//�d��
	m_veloxity.y -= GRAVITY;

	//�p�[�c��Update
	for (auto& part : m_parts) {
		if (part)part->Update();	
	}	
	
	//�p�[�c��TRS�X�V
	for (auto& part : m_parts) {
		if (part)part->UpdateTRS();
	}
	//�R�A��TRS�X�V
	m_coreModel.SetPRS(GetPos(), GetRot(), m_scale);
	SetCollisionPosOffset(CalcCollisionPosOffset(m_scale));
	SetCollisionVelocity(GetMove());
	//���w��TRS�X�V
	m_kouhai.SetPRS(
		GetPos() + (GetBack() * 25000.0f*0.5f * m_scale) + (GetUp() * 25000.0f*0.25f * m_scale),
		GetRot(),
		50000.0f* 0.75f * m_scale
	);

	//�p�[�c�̃��[���h�s��X�V��Update
	for (auto& part : m_parts) {
		if (part)part->PostUTRSUpdate();
	}
}

void CDeathHotoke::PostLoopUpdate() {
	for (auto& part : m_parts) {
		if (part)part->PostLoopUpdate();
	}	
}

void CDeathHotoke::Pre3DRender(int screenNum) {
	//���w���f���𑊎�̉�ʂł̂ݕ`�悳����
	if (!m_isDrawKouhai) {
		return;
	}
	if (m_playerNum != screenNum) {
		m_kouhai.SetIsDraw(true);
	}
	else {
		m_kouhai.SetIsDraw(false);
	}
}

void CDeathHotoke::HUDRender(int HUDNum) {
	if (!m_isDrawHUD || m_playerNum != HUDNum)return;

	if (GetIsDrawHowto()) {
		//����
		if (GetPlayerNum() == 0) {
			GetJapaneseFont()->Draw(L"[R�X�e�B�b�N]\n:���Ă񂢂ǂ�\n[RSB]�o�b�N�݂�", { 0.0f,0.15f });
		}
		else {
			GetJapaneseFont()->Draw(L"[R�X�e�B�b�N]\n:���Ă񂢂ǂ�\n[RSB]�o�b�N�݂�", { 1.0f,0.15f }, { 1.1f,0.0f });
		}
	}

	//�p�[�c��2D�`��
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
}

void CDeathHotoke::Damage(const ReferenceCollision& ref, const CVector3& pos) {
	if (ref.damege < FLT_EPSILON) { return; }//�m�[�_���[�W

	if (!m_isImmortal) {
		m_hp -= ref.damege;
		m_damegePower = max(ref.damege, m_damegePower);
	}

	//SE
	new GameSE(L"Resource/sound/Explosion6.wav", GetPos(), 1000.0f, GetPlayerNum());

	//����
	for (int i = 0; i < 3; i++) {
		CVector3 dir = ref.direction*-1.0f*(40.0f + 400.0f*CMath::RandomZeroToOne());
		CQuaternion(CVector3::AxisX(), CMath::PI_HALF*0.25f - CMath::PI_HALF*0.5f*CMath::RandomZeroToOne()).Multiply(dir);
		CQuaternion(CVector3::AxisY(), CMath::PI_HALF*0.25f - CMath::PI_HALF*0.5f*CMath::RandomZeroToOne()).Multiply(dir);
		new CSmoke(pos, dir, BLOOD_BLACK_COLOR, 200.0f, 1.2f + 0.1f*(1.0f+CMath::RandomZeroToOne()));
	}
	//����
	int reverseBloodNum = CMath::RandomInt() % 3;//�t�����򖗂̐�(0~2)
	for (int i = 0; i < 8; i++) {
		CVector3 move;
		if (i < reverseBloodNum) {
			//�t�����o��
			move = (CVector3::Up() + ref.direction)*(50.0f + 50.0f*CMath::RandomZeroToOne());
		}
		else {
			//�ʏ����
			move = (CVector3::Up() + ref.direction*-1.0f)*(50.0f + 50.0f*CMath::RandomZeroToOne());
		}
		new CBlood(
			//�ʒu
			pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()),
			//�ړ�
			move
		);
	}
}

void CDeathHotoke::Stun() {
	if (!GetIsStun()) {//�X�^�����ĂȂ�
		return;
	}
	m_stunTimeSec -= FRAME_PER_SECOND;

	//�����݉�]
	AddAngularVelocity(CVector3::AxisY(), CMath::DegToRad(10.0f));
	//����
	AddVelocity(CVector3::Down()*50.0f);
}

#include "stdafx.h"
#include "BP_BirdWing.h"
#include "CGameMode.h"

using namespace GameObj;

void BP_BirdWing::InnerStart() {
	m_name = L"�e�o";
	
	//�A�j��
	m_anim[enDefault].Load(L"Resource/animation/birdWing_default.tka");
	m_anim[enFlying].Load(L"Resource/animation/birdWing_Flying.tka");
	m_anim[enBraking].Load(L"Resource/animation/birdWing_brake.tka");
	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/birdwing.cmo", m_anim, enAnimNum, enFbxUpAxisY);
	m_localScale = { 1927.77f / 48.97f, 2377.50f / 60.39f, 2377.50f / 60.39f };
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetTranslucent(0.3f);
		}
	);
	m_model->GetAnimCon().AddAnimationEventListener(
		[&](const wchar_t* clipName, const wchar_t* eventName) {
			std::wstring_view nameView = eventName;
			if (nameView.compare(L"End")==0) {
				m_isAnimEnd = true;//�A�j���[�V�����I���t���O
			}
		}
	);
	//m_shoulderBone[L] = m_model->FindBone(L"Sholder_L");
	//m_shoulderBone[R] = m_model->FindBone(L"Sholder_R");

	//�R���g���[���[
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_BirdWing(this, m_ptrCore);
	}
	else {
		m_controller = new AICon_BirdWing(this, m_ptrCore);
	}
}

void BP_BirdWing::Update() {
	//���t���[���̏�����
	m_isBraking = false;
	m_isYawInput = false;
	m_isPitchInput = false;
	m_pitch = CQuaternion::Identity();
	
	//�����L�^
	float oldAccel = m_accel;

	//�R���g���[���[�ɑ��삳����
	m_controller->Update();

	//�������ĂȂ������猸��
	if (oldAccel >= m_accel) { m_accel -= 2.0f*0.01f*CalcAirScale(m_ptrCore->GetHeightMeter()); }
	m_accel -= 2.0f*0.01f*max(0.0f,m_ptrCore->GetDrag()*0.05f-1.0f);
	if (!m_isYawInput) { m_yawAccel = 0.0f; }
	if (!m_isPitchInput) { m_pitchAccel = 0.0f; }

	//�ړ�����
	CVector3 move = m_ptrCore->GetFront();
	m_pitch.Multiply(move);

	//���x����
	m_accel = max(m_accel, 0.0f);
	if (m_accel > 20.0f && move.y >= 0.0f) {
		m_accel = max(20.0f, m_accel - 1.5f);
	}	

	//�ړ�	
	move *= m_accel;
	m_ptrCore->AddVelocity(move);	

	//�㏸�ɂ�錸��
	if (move.y > 0.0f) {
		m_accel += move.y * -0.003f;
		//���E���x����
		if (m_ptrCore->GetHeightMeter() > LIMIT_HEIGHT_METER) {
			m_accel += move.y * -0.03f;
		}
	}	

	//�A�j���[�V����
	if (m_isAnimEnd && !m_isBraking) { m_model->GetAnimCon().Play(enDefault, 0.175f); }
}

void BP_BirdWing::PostUTRSUpdate() {
	//�����ɂ�����
	if (m_ptrCore->GetMove().y < 0.0f) {
		m_accel += m_ptrCore->GetMove().y * -0.006f;
	}

	//�n�ʌ��ʓx�������Z�o
	float graundEffect = 1.0f;//1.0~
	constexpr float GE_MAX_Y = 300.0f;
	//���C�Ŕ���
	btVector3 rayStart = m_ptrCore->GetFootPos();
	btVector3 rayEnd = m_ptrCore->GetFootPos(); rayEnd.setY(rayEnd.y() - (GE_MAX_Y+20.0f));
	btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
	GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
	if (gnd_ray.hasHit()) {
		graundEffect = 2.0f - min(1.0f, abs(gnd_ray.m_hitPointWorld.y() - m_ptrCore->GetFootPos().y) / GE_MAX_Y);
		//graundEffect *= graundEffect;
		//m_y = graundEffect;
	}

	//�g�͔���	
	CVector3 youryok = m_ptrCore->GetMove(); youryok.y = 0.0f;
	m_ptrCore->AddVelocity(CVector3::Up()*youryok.Length()*0.25f*graundEffect);
}

void BP_BirdWing::Draw2D() {
	if (!m_ptrCore->GetIsDrawHUD()) { return; }

	//����
	//m_ptrCore->GetFont()->DrawFormat(L"ACL(per): %.1f", { 0.0f,0.85f },0.0f, m_accel / 20.0f*100.0f);
	//m_ptrCore->GetFont()->DrawFormat(L"%.1f, %.5f\n%.1f", { 0.0f,0.85f }, 0.0f, m_accel, CalcAirScale(m_ptrCore->GetHeightMeter()),m_y);

	if (m_ptrCore->GetIsDrawHowto()) {
		//����
		m_ptrCore->GetJapaneseFont()->Draw(L"[L�X�e�B�b�N]\n1�����Ă�:������\n���傤��:�s�b�`\n���䂤:���[\n[LSB]�u���[�L", { 0.0f,0.3f });
	}

	//�u���[�L�\��
	if (m_isBraking) {
		m_ptrCore->GetFont()->Draw(L"[BRAKING]", { 0.3f,0.90f }, { 0.0f,0.0f });
	}

	//���E���x�x��
	if (m_ptrCore->GetHeightMeter() > LIMIT_HEIGHT_METER) {
		m_ptrCore->GetWarningFont()->Draw(L"[WARNING-HEIGHT]", { 0.7f,0.90f }, { 1.0f,0.0f });
	}
}

void BP_BirdWing::Accel() {
	if (m_isAnimEnd) {
		m_accel += 20.0f*CalcAirScale(m_ptrCore->GetHeightMeter());
		m_isAnimEnd = false;
		m_model->GetAnimCon().Play(enFlying, 0.175f, true);

		//SE
		new GameSE(L"Resource/sound/wing.wav", m_ptrCore->GetPos(), 150.0f, m_ptrCore->GetPlayerNum());
	}
}
void BP_BirdWing::Brake() {
	if (m_isAnimEnd) {
		m_isBraking = true;
		m_accel -= 2.0f*0.10f*CalcAirScale(m_ptrCore->GetHeightMeter());
		m_isAnimEnd = true;
		m_model->GetAnimCon().Play(enBraking, 0.175f);
	}
}

void BP_BirdWing::Pitch(float lerp) {
	//if (!m_isAnimEnd) { return; }

	m_isPitchInput = true;
	
	//����
	if (signbit(lerp) != signbit(m_pitchAccel)) { m_pitchAccel = 0.0f; }
	m_pitchAccel = copysign(min(1.0f, abs(m_pitchAccel) + (1.0f / 20.0f)), lerp);
	float accel = abs(m_pitchAccel*m_pitchAccel);
	
	//����
	m_accel -= 2.0f*0.01f*abs(lerp)*accel*0.6f*CalcAirScale(m_ptrCore->GetHeightMeter());

	//����
	m_pitch.SetRotation(m_ptrCore->GetLeft(), CMath::PI_HALF*lerp*accel*0.6f);

	//�{�[����]	
	/*CQuaternion rot;
	if (m_isAnimEnd) { rot.SetRotation(CVector3::AxisX(), ); }
	for (auto& bone : m_shoulderBone) {
		bone->SetRotationOffset(rot);
	}	*/
}
void BP_BirdWing::Yaw(float lerp) {
	//if (!m_isAnimEnd) { return; }

	m_isYawInput = true;
	
	//����
	if (signbit(lerp) != signbit(m_yawAccel)) { m_yawAccel = 0.0f; }
	m_yawAccel = copysign(min(1.0f, abs(m_yawAccel) + (1.0f / 20.0f)), lerp);
	float accel = abs(m_yawAccel*m_yawAccel);

	//����
	m_accel -= 2.0f*0.02f*abs(lerp)*accel*CalcAirScale(m_ptrCore->GetHeightMeter());

	//���񑬓x
	float roll = 0.1f, speed = abs(m_accel);
	if (speed < 5.0f) {
		roll *= speed / 5.0f;
	}
	else {
		roll *= 5.0f / speed;
	}
	roll += 0.003f;//�Œ�����
	roll *= 0.2f;
	//if (m_isBraking) { roll *= 2.5f; }//�u���[�L���͐��񐫔\�㏸//TODO(�{�̂̐��񐫔\�p�����[�^�ł��?)

	//����
	m_ptrCore->AddAngularVelocity(CVector3::AxisY(), lerp*roll*accel);
}

void HCon_BirdWing::InnerUpdate() {
	constexpr float ACCEL_TIME_MAX = 0.35f;
	//���X�e�B�b�N���]�ȏ�ŉ���
	if (m_ptrCore->GetPad()->GetStickCircleInput(L) - m_beforeClrcleInputNum > 0) {
		m_accelTime = ACCEL_TIME_MAX;
	}
	//���X�e�B�b�N��]���L�^
	m_beforeClrcleInputNum = m_ptrCore->GetPad()->GetStickCircleInput(L);
	//��莞�ԉ���
	if (m_accelTime > 0.0f) {
		m_accelTime -= FRAME_PER_SECOND;
		m_ptrBody->Accel();
	}

	//�u���[�L
	if (m_ptrCore->GetPad()->GetWingInput()) {
		m_ptrBody->Brake();
	}

	//if (m_accelTime < FLT_EPSILON){// < ACCEL_TIME_MAX - 0.016f*20.0f) {
		//����
		if (abs(m_ptrCore->GetPad()->GetStick(L).x) > FLT_EPSILON) {
			m_ptrBody->Yaw(m_ptrCore->GetPad()->GetStick(L).x);
		}
		if (abs(m_ptrCore->GetPad()->GetStick(L).y) > FLT_EPSILON) {
			m_ptrBody->Pitch(m_ptrCore->GetPad()->GetStick(L).y*-1.0f);
		}
	//}
}

namespace {
	constexpr int MAX_INTERVAL = (FRAME_RATE * 6);
}
AICon_BirdWing::AICon_BirdWing(BP_BirdWing* ptrbody, CDeathHotoke* ptrCore) : IBodyController(ptrbody, ptrCore) {
	if (CMath::IntUniDist(2) == 0) {
		m_count = (int)(MAX_INTERVAL - FRAME_RATE*0.75f);
	}
	else {
		m_count = CMath::RandomInt() % (FRAME_RATE * 2);
	}

	m_isHardAI = !FindGO<CGameMode>(L"CGameMode")->GetIsEasy();
}
void AICon_BirdWing::InnerUpdate() {
	//�Ԋu�ŗ������[�h�؂�ւ�
	m_count++;
	if (m_count > MAX_INTERVAL) {
		m_isDownMode = !m_isDownMode;
		m_downPower = CMath::RandomZeroToOne()*0.3f + 0.5f;
		if (m_isDownMode) {
			m_count = (CMath::RandomInt() % (FRAME_RATE * 2))+ FRAME_RATE*3 ;
		}
		else {
			m_count = CMath::RandomInt() % (FRAME_RATE * 1);
		}
	}

	//�ړI�n�ֈړ�
	if (m_ptrCore->GetAIStatus()->isMovingToTarget) {		
		if (m_isDownMode) {
			//����
			m_ptrBody->Pitch(-m_downPower);
		}
		if (!m_isDownMode || m_isHardAI) {
			//����
			m_ptrBody->Accel();
		}
		
		//����
		CVector3 v = m_ptrCore->GetAIStatus()->moveTargetPosition - m_ptrCore->GetPos();
		float dot = m_ptrCore->GetRight().Dot(v.GetNorm());
		if (abs(dot) > 0.1f) {
			dot /= abs(dot);
		}
		else {
			dot = m_ptrCore->GetFront().Dot(v.GetNorm());
			if (dot < 0.75f) {
				dot = 1.0f;
			}
			else {
				dot = 0.0f;
			}
		}
		m_ptrBody->Yaw(dot);//����		
	}
}
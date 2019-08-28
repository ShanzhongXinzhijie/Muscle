#include "stdafx.h"
#include "BP_BirdWing.h"

using namespace GameObj;

void BP_BirdWing::InnerStart() {
	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/birdwing.cmo");
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetTranslucent(0.3f);
		}
	);

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
	m_pitch = CQuaternion::Identity();

	//����
	m_yawAccel -= 0.01f; m_yawAccel = max(m_yawAccel, 0.0f);

	//�R���g���[���[�ɑ��삳����
	m_controller->Update();

	//���x����
	m_accel = CMath::Clamp(m_accel, 0.0f, 20.0f);	

	//TODO �g�͔���

	//�ړ�
	CVector3 move = m_ptrCore->GetFront()*m_accel;
	m_pitch.Multiply(move);
	m_ptrCore->AddMove(move);
}

void BP_BirdWing::Draw2D() {
	if (!m_ptrCore->GetIsDrawHUD()) { return; }

	CFont font;
	wchar_t output[256];

	swprintf_s(output, L"%.1f", m_accel);
	font.Draw(output, {0.0f,0.5f});
}

void BP_BirdWing::Accel() {
	m_accel += 2.0f*0.08f;
}
void BP_BirdWing::Brake() {
	m_accel -= 2.0f*0.10f;
}

void BP_BirdWing::Pitch(float lerp) {
	//����
	m_pitch.SetRotation(m_ptrCore->GetLeft(), lerp*CMath::PI_HALF*0.8f);
}
void BP_BirdWing::Yaw(float lerp) {
	//���񑬓x
	float roll = 0.1f, speed = abs(m_accel);
	if (speed < 5.0f) {
		roll *= speed / 5.0f;
	}
	else {
		roll *= 5.0f / speed;
	}
	roll += 0.003f;
	m_yawAccel += roll * 0.1f;
	//����
	m_ptrCore->AddRot(CQuaternion(CVector3::AxisY(), lerp*m_yawAccel*2.0f));
}

void HCon_BirdWing::Update() {
	//���X�e�B�b�N���]�ȏ�ŉ�����
	if (m_ptrCore->GetPad()->GetStickCircleInput(L) - m_beforeClrcleInputNum > 0) {
		m_accelTime = 0.3f;		
	}
	m_beforeClrcleInputNum = m_ptrCore->GetPad()->GetStickCircleInput(L);
	//��莞�ԉ���
	if (m_accelTime > 0.0f) {
		m_accelTime -= GetDeltaTimeSec();
		//���v��肩?
		if (m_ptrCore->GetPad()->GetStickCircleenSpinMode(L) == IGamePad::enClockwise) {
			m_ptrBody->Accel();
		}
		else {
			m_ptrBody->Brake();
		}
	}

	//����
	if (abs(m_ptrCore->GetPad()->GetStick(L).x) > FLT_EPSILON) {
		m_ptrBody->Yaw(m_ptrCore->GetPad()->GetStick(L).x);
	}
	if (abs(m_ptrCore->GetPad()->GetStick(L).y) > FLT_EPSILON) {
		m_ptrBody->Pitch(m_ptrCore->GetPad()->GetStick(L).y);		
	}
}

void AICon_BirdWing::Update() {
	if (m_ptrBody->GetAccel() < 5.0f){
		m_ptrBody->Accel();
	}

	//�ړI�n�ֈړ�
	if (m_ptrCore->GetAIStatus()->isMovingToTarget) {
		CVector3 v = m_ptrCore->GetAIStatus()->moveTargetPosition - m_ptrCore->GetPos();
		float dot = m_ptrCore->GetRight().Dot(v.GetNorm());
		if (abs(dot) > 0.1f) {
			dot /= abs(dot);
		}
		else {
			dot = 0.0f;
		}
		m_ptrBody->Yaw(dot);//����
	}
}
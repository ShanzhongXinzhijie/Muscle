#include "stdafx.h"
#include "BP_BirdWing.h"

namespace {
	static const CVector3 SpinDirection[4] = {
		   CVector3(0.0f,1.0f,0.0f),//��
		   CVector3(1.0f,0.0f,0.0f),//�E
		   CVector3(0.0f,-1.0f,0.0f),//��
		   CVector3(-1.0f,0.0f,0.0f)//��
	};
}

using namespace GameObj;

void BP_BirdWing::InnerStart() {
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/birdwing.cmo");
}

void BP_BirdWing::Update() {
	CVector2 stick = m_ptrCore->GetPad()->GetStick(L);
	float length = stick.LengthSq();
	stick.Normalize();

	//���͗P�\�؂�
	m_inputTimer -= 1.0f;
	if (m_inputTimer < FLT_EPSILON) {
		m_nextDirection = enDirNum; m_backDirection = enDirNum; m_spinMode = enNone;
		m_inputTimer = 0.0f; m_spinCount = 0; m_spinCountBefore = 0;
	}

	if (length > CMath::Square(0.75f)) {
		if (m_nextDirection == enDirNum) {
			float angle = 10000.0f;
			for (int i = 0; i < enDirNum; i++) {
				float angle2 = CVector3::AngleOf2NormalizeVector(SpinDirection[i], { stick.x,stick.y,0.0f });
				if (angle > angle2) {
					angle = angle2;
					m_nextDirection = (enSpinDirection)((i + 1 == enDirNum) ? 0 : i + 1);
					m_backDirection = (enSpinDirection)((i - 1 < 0) ? enDirNum - 1 : i - 1);
				}
			}
			m_spinCount = 1;
			m_inputTimer = 30.0f;
			m_spinMode = enNone;
		}
		else {
			enSpinDirection isReflesh = enDirNum;
			float angle;
			//���v���
			angle = CVector3::AngleOf2NormalizeVector(SpinDirection[m_nextDirection], { stick.x,stick.y,0.0f });
			if (angle < CMath::DegToRad(45.0f)) {
				isReflesh = m_nextDirection;					
				if (m_spinMode == enBack) { m_spinCount = 0; m_spinCountBefore = 0; }else { m_spinCount++; }				
				m_inputTimer = 30.0f;
				m_spinMode = enClockwise;
			}
			//�����v���
			angle = CVector3::AngleOf2NormalizeVector(SpinDirection[m_backDirection], { stick.x,stick.y,0.0f });
			if (angle < CMath::DegToRad(45.0f)) {
				isReflesh = m_backDirection;
				if (m_spinMode == enClockwise) { m_spinCount = 0; m_spinCountBefore = 0; }else { m_spinCount++; }
				m_inputTimer = 30.0f;
				m_spinMode = enBack;
			}
			if (isReflesh != enDirNum) {
				m_nextDirection = (enSpinDirection)((isReflesh + 1 == enDirNum) ? 0 : isReflesh + 1);
				m_backDirection = (enSpinDirection)((isReflesh - 1 < 0) ? enDirNum - 1 : isReflesh - 1);
			}
		}
	}
	//���]�ȏ���͍ς݂Ȃ����
	if (m_spinCount >= 4) {
		if (m_spinMode == enClockwise) { m_accel += 0.1f; }	else { m_accel -= 0.1f; }
	}
	//���]���Ƃɉ���
	if (m_spinCountBefore+4 <= m_spinCount){
		if (m_spinMode == enClockwise) { m_accel += 1.0f; }	else { m_accel -= 1.0f; }
		m_spinCountBefore += 4;
	}
	//�ō����x����
	m_accel = CMath::Clamp(m_accel, -10.0f, 20.0f);

	//TODO ��莞�Ԃ̓��͂��K�v
	//�s�b�` TODO
	
	//TODO �g�͔���
	//�������݂Ńu���[�L

	//���[
	//���񑬓x
	float yaw = 0.1f, speed = abs(m_accel);
	if (speed < 5.0f) {
		yaw *= speed / 5.0f;
	}
	else {
		yaw *= 5.0f / speed;
	}
	//����
	CQuaternion rot;
	if (m_spinCount <= 1) {
		rot.SetRotation(CVector3::AxisY(), stick.x*yaw);
	}

	//�ړ�
	CVector3 move = CVector3(0.0f, 0.25f, 1.0f)*m_accel;
	m_ptrCore->GetRot().Multiply(move);

	//�V�������W��ݒ�
	m_ptrCore->AddMove(move);
	//�V������]��ݒ�
	m_ptrCore->AddRot(rot);
}

void BP_BirdWing::Draw2D() {
	CFont font;
	wchar_t output[256];
	swprintf_s(output, L"%.1f\n%d", m_accel, m_spinCount);

	font.Draw(output, {0.0f,0.5f});
}
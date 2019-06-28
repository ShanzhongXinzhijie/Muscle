#include "stdafx.h"
#include "BirdWing.h"

namespace {
	static const CVector3 SpinDirection[4] = {
		   CVector3(0.0f,1.0f,0.0f),//è„
		   CVector3(1.0f,0.0f,0.0f),//âE
		   CVector3(0.0f,-1.0f,0.0f),//â∫
		   CVector3(-1.0f,0.0f,0.0f)//ç∂
	};
}

void BirdWing::SetTRS() {
	m_model.SetPos(m_ptrCore->GetPos());
	m_model.SetRot(m_ptrCore->GetRot());
	m_model.SetScale(m_ptrCore->GetScale());
}

void BirdWing::Start() {
	m_model.Init(L"Resource/modelData/birdwing.cmo");

	SetTRS();
}

void BirdWing::Update() {
	SetTRS();

	CVector2 stick = m_ptrCore->GetPad()->GetStick(L);
	stick.Normalize();

	//ì¸óÕóPó\êÿÇÍ
	m_inputTimer -= 1.0f;
	if (m_inputTimer < FLT_EPSILON) {
		m_nextDirection = enDirNum; m_backDirection = enDirNum; m_spinMode = enNone;
		m_inputTimer = 0.0f; m_spinCount = 0;
	}

	if (stick.LengthSq() > CMath::Square(0.5f)) {
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
			//éûåvâÒÇË
			angle = CVector3::AngleOf2NormalizeVector(SpinDirection[m_nextDirection], { stick.x,stick.y,0.0f });
			if (angle < CMath::DegToRad(45.0f)) {
				isReflesh = m_nextDirection;					
				if (m_spinMode == enBack) { m_spinCount = 0; }else { m_spinCount++; }				
				m_inputTimer = 30.0f;
				m_spinMode = enClockwise;
			}
			//îΩéûåvâÒÇË
			angle = CVector3::AngleOf2NormalizeVector(SpinDirection[m_backDirection], { stick.x,stick.y,0.0f });
			if (angle < CMath::DegToRad(45.0f)) {
				isReflesh = m_backDirection;
				if (m_spinMode == enClockwise) { m_spinCount = 0; }else { m_spinCount++; }
				m_inputTimer = 30.0f;
				m_spinMode = enBack;
			}
			if (isReflesh != enDirNum) {
				m_nextDirection = (enSpinDirection)((isReflesh + 1 == enDirNum) ? 0 : isReflesh + 1);
				m_backDirection = (enSpinDirection)((isReflesh - 1 < 0) ? enDirNum - 1 : isReflesh - 1);
			}
		}
	}
	if (m_spinCount >= 4) {
		if (m_spinMode == enClockwise) {
			m_accel += 0.1f; m_accel = min(m_accel, 20.0f);
		}
		else {
			m_accel -= 0.1f; m_accel = max(m_accel, -10.0f);
		}
		//m_spinCount = 0;
	}

	m_ptrCore->SetPos(m_ptrCore->GetPos() + CVector3(0.0f,0.25f,1.0f)*m_accel);
}

void BirdWing::Draw2D() {
	CFont font;
	wchar_t output[256];
	swprintf_s(output, L"%.1f\n%d", m_accel, m_spinCount);

	font.Draw(output, {0.0f,0.5f});
}
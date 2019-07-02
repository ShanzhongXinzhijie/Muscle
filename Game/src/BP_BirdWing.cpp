#include "stdafx.h"
#include "BP_BirdWing.h"

namespace {
	static const CVector3 SpinDirection[4] = {
		   CVector3(0.0f,1.0f,0.0f),//上
		   CVector3(1.0f,0.0f,0.0f),//右
		   CVector3(0.0f,-1.0f,0.0f),//下
		   CVector3(-1.0f,0.0f,0.0f)//左
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

	//入力猶予切れ
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
			//時計回り
			angle = CVector3::AngleOf2NormalizeVector(SpinDirection[m_nextDirection], { stick.x,stick.y,0.0f });
			if (angle < CMath::DegToRad(45.0f)) {
				isReflesh = m_nextDirection;					
				if (m_spinMode == enBack) { m_spinCount = 0; m_spinCountBefore = 0; }else { m_spinCount++; }				
				m_inputTimer = 30.0f;
				m_spinMode = enClockwise;
			}
			//反時計回り
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
	//一回転以上入力済みなら加速
	if (m_spinCount >= 4) {
		if (m_spinMode == enClockwise) { m_accel += 0.1f; }	else { m_accel -= 0.1f; }
	}
	//一回転ごとに加速
	if (m_spinCountBefore+4 <= m_spinCount){
		if (m_spinMode == enClockwise) { m_accel += 1.0f; }	else { m_accel -= 1.0f; }
		m_spinCountBefore += 4;
	}
	//最高速度制限
	m_accel = CMath::Clamp(m_accel, -10.0f, 20.0f);

	//TODO 一定時間の入力が必要
	//ピッチ TODO
	
	//TODO 揚力発生
	//押し込みでブレーキ

	//ヨー
	//旋回速度
	float yaw = 0.1f, speed = abs(m_accel);
	if (speed < 5.0f) {
		yaw *= speed / 5.0f;
	}
	else {
		yaw *= 5.0f / speed;
	}
	//旋回
	CQuaternion rot;
	if (m_spinCount <= 1) {
		rot.SetRotation(CVector3::AxisY(), stick.x*yaw);
	}

	//移動
	CVector3 move = CVector3(0.0f, 0.25f, 1.0f)*m_accel;
	m_ptrCore->GetRot().Multiply(move);

	//新しい座標を設定
	m_ptrCore->AddMove(move);
	//新しい回転を設定
	m_ptrCore->AddRot(rot);
}

void BP_BirdWing::Draw2D() {
	CFont font;
	wchar_t output[256];
	swprintf_s(output, L"%.1f\n%d", m_accel, m_spinCount);

	font.Draw(output, {0.0f,0.5f});
}
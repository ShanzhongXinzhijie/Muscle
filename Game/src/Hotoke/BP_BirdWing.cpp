#include "stdafx.h"
#include "BP_BirdWing.h"

using namespace GameObj;

namespace {
	float CalcAirScale(float heightMeter) {
		return max(0.0f, 1.0f - max(0.0f, heightMeter / MAX_HEIGHT_METER));//高度MAX_HEIGHT_METERに近づくに連れ空気が薄くなる
	}
}

void BP_BirdWing::InnerStart() {
	//アニメ
	m_anim[enDefault].Load(L"Resource/animation/birdWing_default.tka");
	m_anim[enFlying].Load(L"Resource/animation/birdWing_Flying.tka");
	m_anim[enBraking].Load(L"Resource/animation/birdWing_brake.tka");
	//モデル
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
				m_isAnimEnd = true;//アニメーション終了フラグ
			}
		}
	);
	//m_shoulderBone[L] = m_model->FindBone(L"Sholder_L");
	//m_shoulderBone[R] = m_model->FindBone(L"Sholder_R");

	//コントローラー
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_BirdWing(this, m_ptrCore);
	}
	else {
		m_controller = new AICon_BirdWing(this, m_ptrCore);
	}
}

void BP_BirdWing::Update() {
	//毎フレームの初期化
	m_isBraking = false;
	m_isYawInput = false;
	m_isPitchInput = false;
	m_pitch = CQuaternion::Identity();
	
	//加速記録
	float oldAccel = m_accel;

	//コントローラーに操作させる
	m_controller->Update();

	//加速してなかったら減速
	if (oldAccel >= m_accel) { m_accel -= 2.0f*0.01f*CalcAirScale(m_ptrCore->GetHeightMeter()); }
	m_accel -= 2.0f*0.01f*max(0.0f,m_ptrCore->GetDrag()*0.05f-1.0f);
	if (!m_isYawInput) { m_yawAccel = 0.0f; }
	if (!m_isPitchInput) { m_pitchAccel = 0.0f; }

	//速度制限
	m_accel = max(m_accel, 0.0f);
	if (m_accel > 20.0f) {
		m_accel = max(20.0f, m_accel - 1.5f);
	}	

	//移動
	CVector3 move = m_ptrCore->GetFront()*m_accel;
	m_pitch.Multiply(move);
	m_ptrCore->AddVelocity(move);	

	//上昇による減速
	if (move.y > 0.0f) {
		m_accel += move.y * -0.003f;
	}	

	//アニメーション
	if (m_isAnimEnd && !m_isBraking) { m_model->GetAnimCon().Play(enDefault, 0.175f); }
}

void BP_BirdWing::PostUTRSUpdate() {
	//落下による加速
	if (m_ptrCore->GetMove().y < 0.0f) {
		m_accel += m_ptrCore->GetMove().y * -0.006f;
	}
	//揚力発生	
	CVector3 youryok = m_ptrCore->GetMove(); youryok.y = 0.0f;
	m_ptrCore->AddVelocity(CVector3::Up()*youryok.Length()*0.25f);
}

void BP_BirdWing::Draw2D() {
	if (!m_ptrCore->GetIsDrawHUD()) { return; }

	//加速
	//m_ptrCore->GetFont()->DrawFormat(L"ACL(per): %.1f", { 0.0f,0.85f },0.0f, m_accel / 20.0f*100.0f);

	//ブレーキ表示
	if (m_isBraking) {
		m_ptrCore->GetFont()->Draw(L"[BRAKING]", { 0.5f,0.90f }, { 0.5f,0.0f });
	}
}

void BP_BirdWing::Accel() {
	if (m_isAnimEnd) {
		m_accel += 20.0f*CalcAirScale(m_ptrCore->GetHeightMeter());
		m_isAnimEnd = false;
		m_model->GetAnimCon().Play(enFlying, 0.175f, true);
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
	
	//加速
	if (signbit(lerp) != signbit(m_pitchAccel)) { m_pitchAccel = 0.0f; }
	m_pitchAccel = copysign(min(1.0f, abs(m_pitchAccel) + (1.0f / 20.0f)), lerp);
	float accel = abs(m_pitchAccel*m_pitchAccel);
	
	//減速
	m_accel -= 2.0f*0.01f*abs(lerp)*accel*0.6f*CalcAirScale(m_ptrCore->GetHeightMeter());

	//旋回
	m_pitch.SetRotation(m_ptrCore->GetLeft(), CMath::PI_HALF*lerp*accel*0.6f);

	//ボーン回転	
	/*CQuaternion rot;
	if (m_isAnimEnd) { rot.SetRotation(CVector3::AxisX(), ); }
	for (auto& bone : m_shoulderBone) {
		bone->SetRotationOffset(rot);
	}	*/
}
void BP_BirdWing::Yaw(float lerp) {
	//if (!m_isAnimEnd) { return; }

	m_isYawInput = true;
	
	//加速
	if (signbit(lerp) != signbit(m_yawAccel)) { m_yawAccel = 0.0f; }
	m_yawAccel = copysign(min(1.0f, abs(m_yawAccel) + (1.0f / 20.0f)), lerp);
	float accel = abs(m_yawAccel*m_yawAccel);

	//減速
	m_accel -= 2.0f*0.02f*abs(lerp)*accel*CalcAirScale(m_ptrCore->GetHeightMeter());

	//旋回速度
	float roll = 0.1f, speed = abs(m_accel);
	if (speed < 5.0f) {
		roll *= speed / 5.0f;
	}
	else {
		roll *= 5.0f / speed;
	}
	roll += 0.003f;//最低旋回力
	roll *= 0.2f;
	//if (m_isBraking) { roll *= 2.5f; }//ブレーキ中は旋回性能上昇//TODO(本体の旋回性能パラメータでやる?)

	//旋回
	m_ptrCore->AddAngularVelocity(CVector3::AxisY(), lerp*roll*accel);
}

void HCon_BirdWing::Update() {
	constexpr float ACCEL_TIME_MAX = 0.35f;
	//左スティック一回転以上で加速
	if (m_ptrCore->GetPad()->GetStickCircleInput(L) - m_beforeClrcleInputNum > 0) {
		m_accelTime = ACCEL_TIME_MAX;
	}
	//左スティック回転数記録
	m_beforeClrcleInputNum = m_ptrCore->GetPad()->GetStickCircleInput(L);
	//一定時間加速
	if (m_accelTime > 0.0f) {
		m_accelTime -= FRAME_PER_SECOND;
		m_ptrBody->Accel();
	}

	//ブレーキ
	if (m_ptrCore->GetPad()->GetWingInput()) {
		m_ptrBody->Brake();
	}

	//if (m_accelTime < FLT_EPSILON){// < ACCEL_TIME_MAX - 0.016f*20.0f) {
		//旋回
		if (abs(m_ptrCore->GetPad()->GetStick(L).x) > FLT_EPSILON) {
			m_ptrBody->Yaw(m_ptrCore->GetPad()->GetStick(L).x);
		}
		if (abs(m_ptrCore->GetPad()->GetStick(L).y) > FLT_EPSILON) {
			m_ptrBody->Pitch(m_ptrCore->GetPad()->GetStick(L).y*-1.0f);
		}
	//}
}

void AICon_BirdWing::Update() {
	//if (m_ptrBody->GetAccel() < 5.0f){
		m_ptrBody->Accel();
	//}

	//目的地へ移動
	if (m_ptrCore->GetAIStatus()->isMovingToTarget) {
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
		m_ptrBody->Yaw(dot);//旋回
	}
}
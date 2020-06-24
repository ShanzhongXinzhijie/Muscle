#include "stdafx.h"
#include "BP_BirdWing.h"
#include "CGameMode.h"

using namespace GameObj;

void BP_BirdWing::InnerStart() {
	m_name = L"テバ";
	
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

	//移動方向
	CVector3 move = m_ptrCore->GetFront();
	m_pitch.Multiply(move);

	//速度制限
	m_accel = max(m_accel, 0.0f);
	if (m_accel > 20.0f && move.y >= 0.0f) {
		m_accel = max(20.0f, m_accel - 1.5f);
	}	

	//移動	
	move *= m_accel;
	m_ptrCore->AddVelocity(move);	

	//上昇による減速
	if (move.y > 0.0f) {
		m_accel += move.y * -0.003f;
		//限界高度制限
		if (m_ptrCore->GetHeightMeter() > LIMIT_HEIGHT_METER) {
			m_accel += move.y * -0.03f;
		}
	}	

	//アニメーション
	if (m_isAnimEnd && !m_isBraking) { m_model->GetAnimCon().Play(enDefault, 0.175f); }
}

void BP_BirdWing::PostUTRSUpdate() {
	//落下による加速
	if (m_ptrCore->GetMove().y < 0.0f) {
		m_accel += m_ptrCore->GetMove().y * -0.006f;
	}

	//地面効果度合いを算出
	float graundEffect = 1.0f;//1.0~
	constexpr float GE_MAX_Y = 300.0f;
	//レイで判定
	btVector3 rayStart = m_ptrCore->GetFootPos();
	btVector3 rayEnd = m_ptrCore->GetFootPos(); rayEnd.setY(rayEnd.y() - (GE_MAX_Y+20.0f));
	btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
	GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
	if (gnd_ray.hasHit()) {
		graundEffect = 2.0f - min(1.0f, abs(gnd_ray.m_hitPointWorld.y() - m_ptrCore->GetFootPos().y) / GE_MAX_Y);
		//graundEffect *= graundEffect;
		//m_y = graundEffect;
	}

	//揚力発生	
	CVector3 youryok = m_ptrCore->GetMove(); youryok.y = 0.0f;
	m_ptrCore->AddVelocity(CVector3::Up()*youryok.Length()*0.25f*graundEffect);
}

void BP_BirdWing::Draw2D() {
	if (!m_ptrCore->GetIsDrawHUD()) { return; }

	//加速
	//m_ptrCore->GetFont()->DrawFormat(L"ACL(per): %.1f", { 0.0f,0.85f },0.0f, m_accel / 20.0f*100.0f);
	//m_ptrCore->GetFont()->DrawFormat(L"%.1f, %.5f\n%.1f", { 0.0f,0.85f }, 0.0f, m_accel, CalcAirScale(m_ptrCore->GetHeightMeter()),m_y);

	if (m_ptrCore->GetIsDrawHowto()) {
		//操作
		m_ptrCore->GetJapaneseFont()->Draw(L"[Lスティック]\n1かいてん:かそく\nじょうげ:ピッチ\nさゆう:ヨー\n[LSB]ブレーキ", { 0.0f,0.3f });
	}

	//ブレーキ表示
	if (m_isBraking) {
		m_ptrCore->GetFont()->Draw(L"[BRAKING]", { 0.3f,0.90f }, { 0.0f,0.0f });
	}

	//限界高度警告
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

void HCon_BirdWing::InnerUpdate() {
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
	//間隔で落下モード切り替え
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

	//目的地へ移動
	if (m_ptrCore->GetAIStatus()->isMovingToTarget) {		
		if (m_isDownMode) {
			//落下
			m_ptrBody->Pitch(-m_downPower);
		}
		if (!m_isDownMode || m_isHardAI) {
			//加速
			m_ptrBody->Accel();
		}
		
		//旋回
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
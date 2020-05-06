#include "stdafx.h"
#include "BP_TankLeg.h"
#include "BulletHeader.h"

using namespace GameObj;

namespace {
	const CVector3 hitboxOffset = CVector3::Down()*25.f;
	const CQuaternion hitboxRot = { CVector3::AxisX(),CMath::PI_HALF };
}

void BP_TankLeg::InnerStart() {
	m_name = L"タンク";

	//ステータス変更
	m_ptrCore->MulHPScale(2.0f);

	//カメラ位置
	m_ptrCore->SetFootCameraOffsetPos(CVector3::Front()*m_viewFront);
	m_ptrCore->SetToFootDistance(m_viewHeight);
	
	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/tankleg.cmo");
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
		if (me->EqualMaterialName(L"08 - Default")) {
			//履帯
			me->SetMetallic(0.5f);
			me->SetShininess(0.2f);
		}
		else {
			//本体
			me->SetMetallic(0.75f);
			me->SetShininess(0.6f);
		}
	}
	);

	//視点位置ボーンを取得
	m_eye = m_model->FindBone(L"eye");

	//脚ボーン取得
	m_legs[0][0] = m_model->FindBone(L"leg0s");
	m_legs[0][1] = m_model->FindBone(L"leg0e");
	m_legs[1][0] = m_model->FindBone(L"leg1s");
	m_legs[1][1] = m_model->FindBone(L"leg1e");
	m_legs[2][0] = m_model->FindBone(L"leg2s");
	m_legs[2][1] = m_model->FindBone(L"leg2e");

	m_legs[3][0] = m_model->FindBone(L"leg0s(mirrored)");
	m_legs[3][1] = m_model->FindBone(L"leg0e(mirrored)");
	m_legs[4][0] = m_model->FindBone(L"leg1s(mirrored)");
	m_legs[4][1] = m_model->FindBone(L"leg1e(mirrored)");
	m_legs[5][0] = m_model->FindBone(L"leg2s(mirrored)");
	m_legs[5][1] = m_model->FindBone(L"leg2e(mirrored)");

	//当たり判定(足)
	{
		constexpr float radius = 80.0f;
		constexpr float height = 150.0f;
		const float modelScale = m_ptrCore->GetScale().GetMax() / (0.0188f*2.0f);
		//形状初期化
		m_col.m_collision.CreateCapsule({}, hitboxRot, radius * modelScale, height*modelScale);

		//ID設定
		m_col.m_reference.ownerID = m_ptrCore->GetFuID();
		m_col.m_reference.nonHitID = m_ptrCore->GetFuID();

		//位置
		m_col.SetPos(m_model->GetRot().GetMultiply(hitboxOffset));
		m_col.SetRot(m_model->GetRot()*hitboxRot);
		m_beforePos = CVector3::Zero();

		m_col.m_reference.offsetPos = m_ptrCore->GetFront()*m_viewFront;

		//物理属性
		m_col.m_reference.attributes.set(enPhysical);

		//衝突処理
		m_col.m_reference.m_preCollisionFunc = [&](ReferenceCollision* H) {
			//体当たり
			auto[damege, stunSec] = DHUtil::CalcRamDamege(m_col.m_reference.velocity, H->velocity);
			m_col.m_reference.damege = damege;
			m_col.m_reference.stunTimeSec = stunSec;
			return true;
		};
	}

	//SE
	m_tankSE = std::make_unique<GameSE>(L"Resource/sound/tank.wav", m_ptrCore->GetPos(), 150.0f, m_ptrCore->GetPlayerNum(), true, true);
	m_tankSE->SetIsAutoDelete(false);
	m_tankSE->SetVol(0.0f);

	//コントローラー
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_TankLeg(this, m_ptrCore);
	}
}

void BP_TankLeg::Update() {
	//初期化
	m_actionType = enNone;
	//ステータス変化
	m_ptrCore->MulDrag(5.0f);

	//コントローラーに操作させる
	if (m_controller)m_controller->Update();

	//接地判定
	m_legHitNum = 0;
	float height = 0.0f;
	for (int i = 0; i < 6; i++) {
		//レイで判定
		btVector3 rayStart = m_legs[i][0]->GetPosition();
		btVector3 rayEnd = m_legs[i][1]->GetPosition();
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			m_legHitNum++;
			if (height < gnd_ray.m_hitPointWorld.y() - rayEnd.y()) {
				height = gnd_ray.m_hitPointWorld.y() - rayEnd.y();
			}
		}
	}
	if (m_legHitNum > 0) {
		//本体位置を上に
		m_ptrCore->SetPos(m_ptrCore->GetPos() + CVector3(0.0f, height, 0.0f));
		//m_ptrCore->AddVelocity(CVector3(0.0f, m_ptrCore->GetGravity(), 0.0f));
	}
}

void BP_TankLeg::PostUTRSUpdate() {
	//判定の更新
	m_col.SetPos(m_ptrCore->GetPos() + m_model->GetRot().GetMultiply(hitboxOffset));
	m_col.SetRot(m_model->GetRot()*hitboxRot);
	m_col.SetVelocity(m_ptrCore->GetPos() - m_beforePos);
	m_beforePos = m_ptrCore->GetPos();
	m_col.m_reference.offsetPos = m_ptrCore->GetFront()*m_viewFront;

	//固定カメラ設定
	//m_ptrCore->SetUseFixedCamera(true);
	//m_ptrCore->SetFixedCameraPoint(m_eye->GetPosition()+ m_ptrCore->GetBack()*40.0f + m_ptrCore->GetDown()*33.0f);
	m_ptrCore->SetFootCameraOffsetPos(CVector3::Front()*m_viewFront);
	m_ptrCore->SetToFootDistance(m_viewHeight);

#ifndef DW_MASTER
	if (GetKeyInput(VK_UP)) {
		m_viewHeight += 1.0f;
	}
	if (GetKeyInput(VK_DOWN)) {
		m_viewHeight -= 1.0f;
	}
	if (GetKeyInput(VK_LEFT)) {
		m_viewFront += 1.0f;
	}
	if (GetKeyInput(VK_RIGHT)) {
		m_viewFront -= 1.0f;
	}
#endif

	//接地処理
	if (m_legHitNum > 0) {
		//接地しているなら抵抗UP
		m_ptrCore->MulDrag(20.0f*m_legHitNum + 10.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));
		//回転力低下
		m_ptrCore->MulAngularDrag(3.0f*m_legHitNum + 1.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));
		//m_ptrCore->MulRotatability(1.0f / (3.0f*m_legHitNum + 1.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y)));
		//振動
		m_ptrCore->SetShakePower(0.0015f*max(0.0f, -m_ptrCore->GetTotalVelocity().y - m_ptrCore->GetGravity()));//*(hitnum/6.0f)
	
		//アクション
		float power = m_legHitNum*CMath::Clamp(-m_ptrCore->GetTotalVelocity().y,0.0f,12.5f);
		switch (m_actionType)
		{
		case BP_TankLeg::enAccel://前進
			m_ptrCore->SetMaxLinearVelocity(m_ptrCore->GetFront()*power*0.6f); 
			break;
		case BP_TankLeg::enTurnL://左旋回
			m_ptrCore->AddAngularVelocity(CVector3::AxisY(), -power * 0.001f);
			break;
		case BP_TankLeg::enTurnR://右旋回
			m_ptrCore->AddAngularVelocity(CVector3::AxisY(), power * 0.001f);
			break;
		default:
			break;
		}
		if (m_actionType) {
			m_tankSE->SetPos(m_ptrCore->GetPos());
			m_tankSE->SetVol(1.0f);
		}
		else {
			m_tankSE->SetVol(0.0f);
		}
	}
}

void BP_TankLeg::Draw2D() {
#ifndef DW_MASTER
	m_ptrCore->GetFont()->DrawFormat(L"%.1f\n%.1f", 0.f, 0.f, m_viewFront, m_viewHeight);
#endif

	if (m_legHitNum > 0) {
		m_ptrCore->GetJapaneseFont()->Draw(L"[LT][RT]キャタピラ", { 0.3f,0.95f + 0.01f }, { 0.0f,0.0f });
	}
}

void BP_TankLeg::Accel() {
	m_actionType = enAccel;
}
void BP_TankLeg::Turn(enLR lr) {
	if (lr == L) {
		m_actionType = enTurnL;
	}
	else {
		m_actionType = enTurnR;
	}
}

void HCon_TankLeg::InnerUpdate() {
	if (m_ptrCore->GetPad()->GetLegInput(L) && m_ptrCore->GetPad()->GetLegInput(R)) {
		m_ptrBody->Accel();
	}
	else {
		if (m_ptrCore->GetPad()->GetLegInput(L)) {
			m_ptrBody->Turn(L);
		}
		if (m_ptrCore->GetPad()->GetLegInput(R)) {
			m_ptrBody->Turn(R);
		}
	}
}

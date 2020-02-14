#include "stdafx.h"
#include "BP_TankLeg.h"
#include "BulletHeader.h"

using namespace GameObj;

namespace {
	const CVector3 hitboxOffset = CVector3::Down()*50.f;
}

void BP_TankLeg::InnerStart() {
	m_name = L"タンク";
	
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
	m_legs[0][0] = m_model->FindBone(L"Bone004");
	m_legs[0][1] = m_model->FindBone(L"Bone005");
	m_legs[1][0] = m_model->FindBone(L"Bone007");
	m_legs[1][1] = m_model->FindBone(L"Bone008");
	m_legs[2][0] = m_model->FindBone(L"Bone010");
	m_legs[2][1] = m_model->FindBone(L"Bone011");

	m_legs[3][0] = m_model->FindBone(L"Bone004(mirrored)");
	m_legs[3][1] = m_model->FindBone(L"Bone005(mirrored)");
	m_legs[4][0] = m_model->FindBone(L"Bone007(mirrored)");
	m_legs[4][1] = m_model->FindBone(L"Bone008(mirrored)");
	m_legs[5][0] = m_model->FindBone(L"Bone010(mirrored)");
	m_legs[5][1] = m_model->FindBone(L"Bone011(mirrored)");

	//当たり判定(足)
	//TODO
	{
		constexpr float radius = 50.0f;
		constexpr float height = 50.0f;
		const float modelScale = m_ptrCore->GetScale().GetMax() / (0.0188f*2.0f);
		//形状初期化
		//m_col[lr].m_collision.CreateCapsule({}, {}, radius * modelScale, height*modelScale);
		m_col.m_collision.CreateSphere({}, {}, radius * modelScale);

		//ID設定
		m_col.m_reference.ownerID = m_ptrCore->GetFuID();
		m_col.m_reference.nonHitID = m_ptrCore->GetFuID();

		//位置
		m_col.SetPos(hitboxOffset);
		m_beforePos = CVector3::Zero();

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

	//コントローラー
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_TankLeg(this, m_ptrCore);
	}
}

void BP_TankLeg::Update() {
	//コントローラーに操作させる
	if (m_controller)m_controller->Update();

	//接地判定
	float height = 0.0f;
	for (int i = 0; i < 6; i++) {
		//レイで判定
		btVector3 rayStart = m_legs[i][0]->GetPosition();
		btVector3 rayEnd = m_legs[i][1]->GetPosition();
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			if (height < rayEnd.y() - gnd_ray.m_hitPointWorld.y()) {
				height = rayEnd.y() - gnd_ray.m_hitPointWorld.y();
			}
		}
	}
	//本体位置を上に
	m_ptrCore->SetPos(m_ptrCore->GetPos() + CVector3(0.0f, height, 0.0f));
}

void BP_TankLeg::PostUTRSUpdate() {
	//判定の更新
	m_col.SetPos(m_ptrCore->GetPos() + hitboxOffset);
	m_col.SetVelocity(m_ptrCore->GetPos() - m_beforePos);
	m_beforePos = m_ptrCore->GetPos();

	//固定カメラ設定
	m_ptrCore->SetUseFixedCamera(true);
	m_ptrCore->SetFixedCameraPoint(m_eye->GetPosition());
}

void BP_TankLeg::Draw2D() {
}

void HCon_TankLeg::InnerUpdate() {
}

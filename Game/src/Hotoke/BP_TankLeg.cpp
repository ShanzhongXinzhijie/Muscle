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
		me->SetMetallic(0.75f);
		me->SetShininess(0.6f);
	}
	);

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
}

void BP_TankLeg::PostUTRSUpdate() {
	//判定の更新
	m_col.SetPos(m_ptrCore->GetPos() + hitboxOffset);
	m_col.SetVelocity(m_ptrCore->GetPos() - m_beforePos);
	m_beforePos = m_ptrCore->GetPos();
}

void BP_TankLeg::Draw2D() {
}

void HCon_TankLeg::InnerUpdate() {
}

#include "stdafx.h"
#include "BP_HumanLeg.h"
#include "BulletKani.h"

using namespace GameObj;

void BP_HumanLeg::InnerStart() {
	//アニメーション(ボーン初期化用)
	m_initPose.Load(L"Resource/animation/humanleg.tka", true);
	
	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/humanleg.cmo", &m_initPose, 1);
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetTranslucent(0.1f);
		}
	);

	//足のIK設定
	//
	m_ikSetting[0] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[0]->tipBone = m_model->FindBone(L"Bone022");
	m_ikSetting[0]->rootBone = m_model->FindBone(L"Bone019");
	m_ikSetting[0]->InitFootIK();
	m_ikSetting[0]->footIKRayEndOffset = CVector3::AxisY()*-2500.0f*m_ptrCore->GetScale().y;
	//
	m_ikSetting[1] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[1]->tipBone = m_model->FindBone(L"Bone022(mirrored)");
	m_ikSetting[1]->rootBone = m_model->FindBone(L"Bone019(mirrored)");
	m_ikSetting[1]->InitFootIK();
	m_ikSetting[1]->footIKRayEndOffset = CVector3::AxisY()*-2500.0f*m_ptrCore->GetScale().y;

	//当たり判定(足)
	constexpr float radius = 50.0f;
	const float modelScale = m_ptrCore->GetScale().GetMax() / (0.0188f*2.0f);
	for (auto lr : {L,R}) {
		m_col[lr].m_collision.CreateSphere(m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo()), {}, radius * modelScale);
		m_col[lr].m_reference.position = m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo());
		m_col[lr].m_reference.attributes.set(enPhysical);
		m_col[lr].m_reference.m_preCollisionFunc = [&,lr](ReferenceCollision* H) { 
			m_col[lr].m_reference.damege = DHUtil::CalcRamDamege(m_col[lr].m_reference.velocity, H->velocity);
		};
	}

	//コントローラー
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_HumanLeg(this, m_ptrCore);
	}
}

void BP_HumanLeg::Update() {
	//コントローラーに操作させる
	if(m_controller)m_controller->Update();

	//足の位置取得
	float footDistance;
	footDistance = m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()).y;
	footDistance = min(footDistance, m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[1]->tipBone->GetNo()).y);

	//足と本体の位置が近ければ、本体位置を上へ
	const float modelScale = m_ptrCore->GetScale().y / (0.0188f*2.0f);
	const float minFootDistance = 20.0f*modelScale, maxFootDistance = 167.0f*modelScale;
	//これ以上、足は縮まない
	if (footDistance < minFootDistance) {
		//本体位置を上に
		m_ptrCore->SetPos(m_ptrCore->GetPos() + CVector3(0.0f, minFootDistance - footDistance,0.0f));
	}
	//足が縮んでいる
	if (footDistance < maxFootDistance) {
		//徐々に足を伸ばす
		float stretchMax = maxFootDistance - footDistance;
		m_ptrCore->AddVelocity(CVector3(0.0f, min(stretchMax, m_ptrCore->GetGravity() + 1.0f), 0.0f));		
	}	
}

void BP_HumanLeg::PostUTRSUpdate() {
	//判定の更新
	for (auto lr : LR) {
		m_col[lr].SetPos(m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo()));
		m_col[lr].SetVelocity(m_ptrCore->GetMove());
	}

	//足の位置取得
	float footDistance;
	footDistance = m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()).y;
	footDistance = min(footDistance, m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[1]->tipBone->GetNo()).y);
	
	const float modelScale = m_ptrCore->GetScale().y / (0.0188f*2.0f);
	const float minFootDistance = 20.0f*modelScale, maxFootDistance = 167.0f*modelScale;
	//ある程度足が縮んでいるなら接地している扱い
	if (footDistance < maxFootDistance - 1.0f) {
		//接地しているなら抵抗UP
		m_ptrCore->MulDrag(20.0f + 10.0f*max(0.0f,-m_ptrCore->GetTotalVelocity().y));
		m_ptrCore->MulRotatability(2.0f+ 1.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));//回転力もUP
		//ジャンプ
		if (m_isJump) {
			float power = 25.0f + 38.0f*(1.0f - abs((minFootDistance - footDistance) / (minFootDistance - maxFootDistance)));
			m_ptrCore->SetMaxLinearVelocity(CVector3::Up()*power);
		}
	}
	
	m_isJump = false;
}

void BP_HumanLeg::Jump() {
	m_isJump = true;
}

void BP_HumanLeg::Stomp() {
	BulletGO* bullet = new BulletGO(
		m_ptrCore->GetTargetPos()+ CVector3::Up()*150.0f,
		CVector3::Down()*5.0f
	);
	bullet->AddComponent(std::make_unique<BD_LegModel>(m_ptrCore->GetRot(), m_ptrCore->GetScale()));
}

void HCon_HumanLeg::Update() {
	if (m_ptrCore->GetPad()->GetLegDown()) {
		m_ptrBody->Jump();
		m_ptrBody->Stomp();
	}
}
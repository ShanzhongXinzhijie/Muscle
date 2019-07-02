#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"

bool CDeathHotoke::Start() {
	//スケール
	constexpr float modelScale = 0.0188f*2.0f;
	m_scale = { modelScale };

	//コア生成
	m_coreModel.Init(L"Resource/modelData/core.cmo");	

	//位置初期化
	m_pos = CVector3::AxisY()*1000.0f;
	m_pos.z += 200.0f;
	
	//パーツ生成
	m_parts[enWing] = new BP_BirdWing(this);
	m_parts[enLeg] = new BP_HumanLeg(this);
	m_parts[enArm] = new BP_KaniArm(this);
	m_parts[enHead] = new BP_FishHead(this);
	//パーツのStart
	for (auto& part : m_parts) {
		if (part)part->Start();
	}

	return true;
}

void CDeathHotoke::Update() {
	//移動適応
	m_pos += m_move;
	m_rot = m_rotMove * m_rot;

	m_move *= 0.5f;
	m_rotMove.Slerp(0.5f, m_rotMove, CQuaternion::Identity());

	//パーツのUpdate
	for (auto& part : m_parts) {
		if (part)part->Update();
		
		//パーツのTRS更新
		for (auto& part : m_parts) {
			if (part)part->UpdateTRS();
		}
		//コアのTRS更新
		m_coreModel.SetPRS(m_pos, m_rot, m_scale);
	}	
}

void CDeathHotoke::PostRender() {
	//パーツの2D描画
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
}
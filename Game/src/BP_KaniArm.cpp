#include "stdafx.h"
#include "BP_KaniArm.h"

using namespace GameObj;

void BP_KaniArm::InnerStart() {
	//アニメーション(ボーン初期化用)
	m_initPose.Load(L"Resource/animation/kaniarm.tka", true);

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/kaniarm.cmo", &m_initPose, 1);

	//腕のIK設定
	m_ikSetting[R] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[R]->tipBone = m_model->FindBone(L"Bone031");
	m_ikSetting[R]->rootBone = m_model->FindBone(L"Bone026");
	m_ikSetting[R]->iteration = 1;
	//m_ikSetting[R]->footIKRayEndOffset = 

	m_ikSetting[L] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[L]->tipBone = m_model->FindBone(L"Bone031(mirrored)");
	m_ikSetting[L]->rootBone = m_model->FindBone(L"Bone026(mirrored)");
	m_ikSetting[L]->iteration = 1;

	//
	m_muzzleBoneID[R] = m_model->FindBoneID(L"Bone031");
	m_muzzleBoneID[L] = m_model->FindBoneID(L"Bone031(mirrored)");

	//エフェクト
	m_muzzleFlash.Init(L"Resource/spriteData/kaniFlare.png", 1, false);
	m_muzzleFlash.GetModel().InitPostDraw(PostDrawModelRender::enAdd);
	m_muzzleFlash.GetModel().GetSkinModel().SetCullMode(D3D11_CULL_NONE);
	m_muzzleFlash.GetModel().SetIsShadowCaster(false);
}

void BP_KaniArm::Update() {
	m_ikSetting[0]->targetPos = CVector3::Zero();
	m_ikSetting[1]->targetPos = CVector3::Zero();

	m_muzzleCnt++;
	if (m_muzzleCnt <= 4) {
		m_muzzleFlash.SetIsDraw(true);
		m_muzzleFlash.SetPos(m_model->GetBonePos(m_muzzleBoneID[L]));
		m_muzzleFlash.SetScaleHoldAspectRatio(100.0f);
	}
	else {
		m_muzzleFlash.SetIsDraw(false);
		if (m_muzzleCnt >= 8) { m_muzzleCnt = 0; }
	}
}
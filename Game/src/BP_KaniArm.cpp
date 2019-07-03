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
	/*setting = m_model[4].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031");
	setting->rootBone = m_model[4].FindBone(L"Bone026");

	setting = m_model[4].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031(mirrored)");
	setting->rootBone = m_model[4].FindBone(L"Bone026(mirrored)");*/

	//エフェクト
	m_muzzleFlash.Init(L"Resource/spriteData/kaniFlare.png");// , 1, false);
	D3D_SHADER_MACRO macros[] = { "TEXTURE", "1", NULL, NULL };
	m_ps.Load("Preset/shader/model.fx", "PSMain_SozaiNoAzi", Shader::EnType::PS, "TEXTURE", macros);
	m_muzzleFlash.GetModel().SetIsPostDraw(true, PostDrawModelRender::enAdd);
	m_muzzleFlash.GetModel().GetSkinModel().FindMaterialSetting([&](MaterialSetting* mat) {mat->SetPS(&m_ps); });
}

void BP_KaniArm::Update() {
	m_muzzleFlash.SetPos(m_ptrCore->GetPos());
	m_muzzleFlash.SetScale({ 1000.0f, 500.0f,1000.0f });
}
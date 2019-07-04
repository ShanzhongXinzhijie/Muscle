#include "stdafx.h"
#include "BP_KaniArm.h"

using namespace GameObj;

void BP_KaniArm::InnerStart() {
	//�A�j���[�V����(�{�[���������p)
	m_initPose.Load(L"Resource/animation/kaniarm.tka", true);

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/kaniarm.cmo", &m_initPose, 1);

	//�r��IK�ݒ�
	/*setting = m_model[4].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031");
	setting->rootBone = m_model[4].FindBone(L"Bone026");

	setting = m_model[4].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031(mirrored)");
	setting->rootBone = m_model[4].FindBone(L"Bone026(mirrored)");*/

	//�G�t�F�N�g
	m_muzzleFlash.Init(L"Resource/spriteData/kaniFlare.png", 1, false);
	m_muzzleFlash.GetModel().InitPostDraw(PostDrawModelRender::enAdd);
	m_muzzleFlash.GetModel().GetSkinModel().SetCullMode(D3D11_CULL_NONE);
}

void BP_KaniArm::Update() {
	m_muzzleFlash.SetPos(m_ptrCore->GetPos());
	m_muzzleFlash.SetScaleHoldAspectRatio(100.0f);
}
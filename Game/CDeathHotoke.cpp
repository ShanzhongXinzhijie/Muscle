#include "stdafx.h"
#include "CDeathHotoke.h"

CDeathHotoke::CDeathHotoke()
{
}


CDeathHotoke::~CDeathHotoke()
{
}

bool CDeathHotoke::Start() {
	m_animationClip[0].Load(L"Resource/animation/humanleg.tka", true);
	m_animationClip[1].Load(L"Resource/animation/kaniarm.tka", true);

	m_model[0].Init(L"Resource/modelData/core.cmo");
	m_model[1].Init(L"Resource/modelData/humanleg.cmo", &m_animationClip[0], 1);
	m_model[2].Init(L"Resource/modelData/fishhead.cmo");
	m_model[3].Init(L"Resource/modelData/birdwing.cmo");
	m_model[4].Init(L"Resource/modelData/kaniarm.cmo", &m_animationClip[1], 1);
	for (auto& model : m_model) {
		model.SetScale(CVector3::One()*0.01f);
	}
	m_pos = CVector3::AxisY()*1000.0f;

	SkeletonIK::IKSetting* setting;
	//足のIK設定
	setting = m_model[1].GetSkinModel().GetSkeleton().GetSkeletonIK().CreateIK();
	setting->tipBone = m_model[1].FindBone(L"Bone022");
	setting->rootBone = m_model[1].FindBone(L"Bone019");
	setting->InitFootIK();
	setting->footIKRayEndOffset = CVector3::AxisY()*-2500.0f*0.01f;

	setting = m_model[1].GetSkinModel().GetSkeleton().GetSkeletonIK().CreateIK();
	setting->tipBone = m_model[1].FindBone(L"Bone022(mirrored)");
	setting->rootBone = m_model[1].FindBone(L"Bone019(mirrored)");
	setting->InitFootIK();
	setting->footIKRayEndOffset = CVector3::AxisY()*-2500.0f*0.01f;
	
	//腕のIK設定
	setting = m_model[4].GetSkinModel().GetSkeleton().GetSkeletonIK().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031");
	setting->rootBone = m_model[4].FindBone(L"Bone026");
	//setting->InitFootIK();
	
	setting = m_model[4].GetSkinModel().GetSkeleton().GetSkeletonIK().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031(mirrored)");
	setting->rootBone = m_model[4].FindBone(L"Bone026(mirrored)");
	//setting->InitFootIK();

	//当たり判定(足)
	const float radius = 50.0f;
	m_col.CreateSphere(m_pos, {}, radius);
	m_col.SetName(L"DH_foot");
	m_col.SetClass(this);

	return true;
}

void CDeathHotoke::Update() {

	CVector3 moveDir;
	if (GetKeyInput('W')) {
		moveDir += m_cam.GetFront();
	}
	if (GetKeyInput('S')) {
		moveDir += m_cam.GetFront()*-1.0f;
	}
	if (GetKeyInput('A')) {
		moveDir += m_cam.GetLeft();
	}
	if (GetKeyInput('D')) {
		moveDir += m_cam.GetLeft()*-1.0f;
	}	
	moveDir.y = 0.0f;
	moveDir.Normalize();
	m_pos += moveDir * 10.0f;

	if (GetKeyInput(VK_SPACE)) {
		m_pos.y += 2.0f;
	}
	if (GetKeyInput('C')) {
		m_pos.y -= 2.0f;
	}

	for (auto& model : m_model) {
		model.SetPos(m_pos);
	}
	m_cam.SetPos(m_pos);

	//判定の更新
	m_col.SetPosition(m_pos);
}
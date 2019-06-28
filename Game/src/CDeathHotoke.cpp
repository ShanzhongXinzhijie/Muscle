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
	const float modelScale = 0.0188f*2.0f;
	for (auto& model : m_model) {
		model.SetScale(modelScale);
		//model.SetIsDrawBoundingBox(true);
	}
	m_pos = CVector3::AxisY()*1000.0f;
	m_pos.z += 200.0f;

	SkeletonIK::IKSetting* setting;
	//‘«‚ÌIKİ’è
	setting = m_model[1].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[1].FindBone(L"Bone022");
	setting->rootBone = m_model[1].FindBone(L"Bone019");
	setting->InitFootIK();
	setting->footIKRayEndOffset = CVector3::AxisY()*-2500.0f*modelScale;

	setting = m_model[1].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[1].FindBone(L"Bone022(mirrored)");
	setting->rootBone = m_model[1].FindBone(L"Bone019(mirrored)");
	setting->InitFootIK();
	setting->footIKRayEndOffset = CVector3::AxisY()*-2500.0f*modelScale;
	
	//˜r‚ÌIKİ’è
	/*setting = m_model[4].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031");
	setting->rootBone = m_model[4].FindBone(L"Bone026");
	
	setting = m_model[4].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031(mirrored)");
	setting->rootBone = m_model[4].FindBone(L"Bone026(mirrored)");*/
	
	//“–‚½‚è”»’è(‘«)
	const float radius = 50.0f;
	m_col.CreateSphere(m_pos, {}, radius);
	m_col.SetName(L"DH_foot");
	m_col.SetClass(this);

	return true;
}

void CDeathHotoke::Update() {

	for (auto& model : m_model) {
		model.SetPos(m_pos);
	}
	//m_cam.SetPos(m_pos);

	//”»’è‚ÌXV
	m_col.SetPosition(m_pos);
}
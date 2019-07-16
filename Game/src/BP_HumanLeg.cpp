#include "stdafx.h"
#include "BP_HumanLeg.h"

using namespace GameObj;

void BP_HumanLeg::InnerStart() {
	//�A�j���[�V����(�{�[���������p)
	m_initPose.Load(L"Resource/animation/humanleg.tka", true);
	
	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/humanleg.cmo", &m_initPose, 1);

	//����IK�ݒ�
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

	//�����蔻��(��)
	constexpr float radius = 50.0f;
	const float modelScale = m_ptrCore->GetScale().GetMax() / (0.0188f*2.0f);
	for (auto lr : {L,R}) {
		m_col[lr].m_collision.CreateSphere(m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo()), {}, radius * modelScale);
		m_col[lr].m_reference.position = m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo());
		m_col[lr].m_reference.attributes.set(enPhysical);
	}
}

void BP_HumanLeg::Update() {
	//���̈ʒu�擾
	float footDistance;
	footDistance = m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()).y;
	footDistance = min(footDistance, m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[1]->tipBone->GetNo()).y);

	//���Ɩ{�̂̈ʒu���߂���΁A�{�̈ʒu�����
	const float modelScale = m_ptrCore->GetScale().y / (0.0188f*2.0f);
	const float minFootDistance = 20.0f*modelScale, maxFootDistance = 167.0f*modelScale;
	if (footDistance < minFootDistance) {
		//����ȏ�A���͏k�܂Ȃ�
		m_ptrCore->SetPos(m_ptrCore->GetPos() + CVector3(0.0f, minFootDistance - footDistance,0.0f));
	}
	if (footDistance < maxFootDistance) {
		//���X�ɑ���L�΂�
		float stretchMax = maxFootDistance - footDistance;
		m_ptrCore->AddMove(CVector3(0.0f, min(stretchMax, 5.0f), 0.0f));
	}	
}

void BP_HumanLeg::PostUTRSUpdate() {
	//����̍X�V
	for (auto lr : { L,R }) {
		m_col[lr].SetPos(m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo()));
	}
}
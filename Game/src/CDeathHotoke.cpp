#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"

bool CDeathHotoke::Start() {
	//�X�P�[��
	constexpr float modelScale = 0.0188f*2.0f;
	m_scale = { modelScale };

	//�R�A����
	m_coreModel.Init(L"Resource/modelData/core.cmo");	

	//�ʒu������
	m_pos = CVector3::AxisY()*1000.0f;
	m_pos.z += 200.0f;
	
	//�p�[�c����
	m_parts[enWing] = new BP_BirdWing(this);
	m_parts[enLeg] = new BP_HumanLeg(this);
	m_parts[enArm] = new BP_KaniArm(this);
	m_parts[enHead] = new BP_FishHead(this);
	//�p�[�c��Start
	for (auto& part : m_parts) {
		if (part)part->Start();
	}

	return true;
}

void CDeathHotoke::Update() {
	//�ړ��K��
	m_pos += m_move;
	m_rot = m_rotMove * m_rot;

	m_move *= 0.5f;
	m_rotMove.Slerp(0.5f, m_rotMove, CQuaternion::Identity());

	//�p�[�c��Update
	for (auto& part : m_parts) {
		if (part)part->Update();
		
		//�p�[�c��TRS�X�V
		for (auto& part : m_parts) {
			if (part)part->UpdateTRS();
		}
		//�R�A��TRS�X�V
		m_coreModel.SetPRS(m_pos, m_rot, m_scale);
	}	
}

void CDeathHotoke::PostRender() {
	//�p�[�c��2D�`��
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
}
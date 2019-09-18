#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"
#include "BP_HumanMantle.h"

void CDeathHotoke::SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part) {
	m_parts[partsType] = std::move(part);
	m_parts[partsType]->Start();
}

void CDeathHotoke::CalcDirection() {
	m_front = CVector3::Front(); GetRot().Multiply(m_front); m_back = m_front * -1.0f;
	m_left = CVector3::Left(); GetRot().Multiply(m_left); m_right = m_left * -1.0f;
	m_up = CVector3::Up(); GetRot().Multiply(m_up); m_down = m_up * -1.0f;
}

bool CDeathHotoke::Start() {
	//�X�P�[��
	constexpr float modelScale = 0.0188f*2.0f;
	m_scale = { modelScale };

	//�R�A����
	m_coreModel.Init(L"Resource/modelData/core.cmo");	
	//�m�[�}���}�b�v�K�p
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;	
	TextureFactory::GetInstance().Load(L"Resource/normalMap/ngasi.png", nullptr, &textureView, nullptr, true);
	m_coreModel.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.9f);
			mat->SetTranslucent(0.2f);
		}
	);

	//�����蔻��
	m_col.m_collision.CreateCapsule({}, {}, 60.0f*(m_scale.x / (0.0188f*2.0f)), 100.0f*(m_scale.y / (0.0188f*2.0f)));// .CreateMesh({}, {}, m_scale, m_coreModel.GetSkinModel());
	m_col.m_collision.SetIsHurtCollision(true);//����͋�炢����
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//�N���X���o��
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				//�_���[�W
				Damage(*H);
			}
		}
	);
	
	//�ʒu������
	m_pos = CVector3::AxisY()*1000.0f;
	m_pos.z += 200.0f;
	m_posOld = m_pos;
	
	return true;
}

void CDeathHotoke::PreUpdate() {
	//�����W�L�^
	m_posOld = m_pos;
}

void CDeathHotoke::Update() {
	//AI���s
	if (m_ai) { m_ai->Update(); }

	//�ړ��K��
	m_pos += m_move;
	m_rot = m_rotMove * m_rot;
	//����
	m_move *= 0.5f;
	m_rotMove.Slerp(0.5f, m_rotMove, CQuaternion::Identity());
	//�d��
	//m_move.y -= 10.0f;

	//�p�[�c��Update
	for (auto& part : m_parts) {
		if (part)part->Update();	
	}	
	//�p�[�c��TRS�X�V
	for (auto& part : m_parts) {
		if (part)part->UpdateTRS();
	}
	//�R�A��TRS�X�V
	m_coreModel.SetPRS(m_pos, m_rot, m_scale);
	//�����x�N�g���X�V
	CalcDirection();
	//�R�A�̃R���W�����X�V
	CVector3 offset = CVector3(0.0f, 60.0f*(m_scale.y / (0.0188f*2.0f)), -15.0f*(m_scale.z / (0.0188f*2.0f)));
	m_rot.Multiply(offset);
	m_col.SetPos(m_pos + offset); m_col.SetRot(m_rot);
	//�p�[�c�̃��[���h�s��X�V��A�b�v�f�[�g
	for (auto& part : m_parts) {
		if (part)part->PostUTRSUpdate();
	}
}

void CDeathHotoke::PostLoopUpdate() {
	for (auto& part : m_parts) {
		if (part)part->PostLoopUpdate();
	}
}

void CDeathHotoke::PostRender() {
	//�p�[�c��2D�`��
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
	//�X�e�[�^�X�`��
	if (!m_isDrawHUD) {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"%.1f", m_hp);
		font.Draw(output, { 0.0f,0.25f });
	}
}

void CDeathHotoke::Damage(const ReferenceCollision& ref) {
	m_hp -= ref.damege;
}
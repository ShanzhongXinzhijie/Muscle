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
	m_col.m_collision.CreateMesh({}, {}, m_scale, m_coreModel.GetSkinModel());
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
	//����
	m_move *= 0.5f;
	m_rotMove.Slerp(0.5f, m_rotMove, CQuaternion::Identity());
	//�d��
	m_move.y -= 1.0f;

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
	m_front = CVector3::Front(); GetRot().Multiply(m_front);
	m_left = CVector3::Left(); GetRot().Multiply(m_left);
	m_up = CVector3::Up(); GetRot().Multiply(m_up);
	//�R�A�̃R���W�����X�V
	m_col.SetPos(m_pos); m_col.SetRot(m_rot);
	//�p�[�c�̃��[���h�s��X�V��A�b�v�f�[�g
	for (auto& part : m_parts) {
		if (part)part->PostUTRSUpdate();
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
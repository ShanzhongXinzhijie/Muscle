#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"
#include "BP_HumanMantle.h"

void CDeathHotoke::SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part) {
	m_parts[partsType] = std::move(part);
	if (GetIsStart()) { m_parts[partsType]->Start(); }
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
	CreateCapsule({}, {}, 60.0f*(m_scale.x / (0.0188f*2.0f)), 100.0f*(m_scale.y / (0.0188f*2.0f)));
	SetCollisionFunc([&](ReferenceCollision* H) { Damage(*H); });
	SetCollisionPos({ 0.0f, 60.0f*(m_scale.y / (0.0188f*2.0f)), -15.0f*(m_scale.z / (0.0188f*2.0f)) });
	
	//�ʒu������	
	SetPos(CVector3::AxisY()*1000.0f + CVector3::AxisZ()*200.0f);
	m_posOld = GetPos();

	//�p�[�c��Start
	for (auto& part : m_parts) {
		if (part)part->Start();
	}
	
	return true;
}

void CDeathHotoke::PreUpdate() {
	//�����W�L�^
	m_posOld = GetPos();
}

void CDeathHotoke::Update() {
	//AI���s
	if (m_ai) { m_ai->Update(); }

	//�ړ��K��
	Move(m_move);
	SetRot(m_rotMove * GetRot());
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
	m_coreModel.SetPRS(GetPos(), GetRot(), m_scale);
	SetCollisionPos({ 0.0f, 60.0f*(m_scale.y / (0.0188f*2.0f)), -15.0f*(m_scale.z / (0.0188f*2.0f)) });
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

	//TODO
	//�o���G�t�F�N�g
	//IFu�p�����p�[�e�B�N����
	//���肠��
	
	//CColObj�̒n�`����

	//�p�[�e�B�N����]
	//dissolve
	//�ԐF

	//�����Ԃ��̉J
	//��ɔ��ŉ��ɗ�����


	//�r���{�[�h�ǂݍ���
	std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>();
	billboard->Init(L"Resource/spriteData/smoke.png");
	billboard->GetModel().InitPostDraw(PostDrawModelRender::enAlpha);
	billboard->SetPos(m_model->GetBonePos(m_muzzleBoneID[i]));
	billboard->SetScale(20.0f);
	//�p�[�e�B�N����
	SuicideObj::CParticle<CBillboard>* particle = new SuicideObj::CParticle<CBillboard>(std::move(billboard), MACHINE_GUN_CHARGE_TIME);
	CVector3 move = CVector3::AxisX()*-40.0f; m_model->GetBoneRot(m_muzzleBoneID[i]).Multiply(move);
	particle->SetMove(move);
	particle->SetScaling(1.2f);
}
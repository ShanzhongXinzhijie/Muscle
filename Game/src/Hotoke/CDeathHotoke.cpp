#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"
#include "BP_HumanMantle.h"

#include "CSmoke.h"
#include "CBlood.h"

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
	SetCollisionFunc(
		[&](ReferenceCollision* H, SuicideObj::CCollisionObj::SCallbackParam& p) {
			CVector3 pos = p.m_collisionPoint;
			Damage(*H,pos);
		}
	);
	SetCollisionPos({ 0.0f, 60.0f*(m_scale.y / (0.0188f*2.0f)), -15.0f*(m_scale.z / (0.0188f*2.0f)) });
	GetAttributes().set(enPhysical);

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

	//�X�e�[�^�X�X�V
	m_drag[enNow] = m_drag[enNext];
	m_angularDrag[enNow] = m_angularDrag[enNext];
	m_rotatability[enNow] = m_rotatability[enNext];
	m_drag[enNext] = 1.0f;
	m_angularDrag[enNext] = 1.0f;
	m_rotatability[enNext] = 1.0f;
}

void CDeathHotoke::Update() {
	//AI���s
	if (m_ai) { m_ai->Update(); }

	//�ړ��K��
	Move(m_veloxity);
	SetRot(m_angularVelocity * GetRot());
	//����
	m_veloxity *= 0.5f*(1.0f/m_drag[enNow]);
	m_angularVelocity.Slerp(0.5f*(1.0f / m_drag[enNow])*(1.0f / m_angularDrag[enNow]), CQuaternion::Identity(), m_angularVelocity);
	//�d��
	m_veloxity.y -= GRAVITY;

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

	//�p�[�c�̃��[���h�s��X�V��Update
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
	/*if (!m_isDrawHUD) {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"%.1f", m_hp);
		font.Draw(output, { 0.0f,0.25f });
	}
	else {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"(%.1f,%.1f,%.1f)\n%.1f", m_veloxity.x, m_veloxity.y, m_veloxity.z, m_drag[enNow]);
		font.Draw(output, { 0.5f,0.25f }, 1.0f, 0.5f);
	}*/
}

void CDeathHotoke::Damage(const ReferenceCollision& ref, const CVector3& pos) {
	if (ref.damege < FLT_EPSILON) { return; }//�m�[�_���[�W

	m_hp -= ref.damege;

	//TODO
	//�o���G�t�F�N�g
	//IFu�p�����p�[�e�B�N����
	//���肠��
	//�t�\�t�g�p�[�e�B�N���̌���

	new CSmoke(pos, ref.direction*-1.0f, { 1.0f,0.0f,0.02f,1.0f });
	new CBlood(pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()), (CVector3::Up() + ref.direction*-1.0f)*50.0f);
	new CBlood(pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()), (CVector3::Up() + ref.direction*-1.0f)*50.0f);
}
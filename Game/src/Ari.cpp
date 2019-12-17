#include "stdafx.h"
#include "Ari.h"

#include "Effect.h"
#include "BulletKani.h"

bool Ari::Start() {
	//�A�j���[�V����
	m_animation.Load(L"Resource/animation/ari/fly.tka", true);

	//���f��
	m_model.Init(L"Resource/modelData/ari.cmo", &m_animation, 1);
	m_model.SetScale(0.05f);

	//�m�[�}���}�b�v 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normaltex;
	TextureFactory::GetInstance().Load(L"Resource/normalMap/n_aritex.png", nullptr, &normaltex, nullptr, true);
	m_model.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(normaltex.Get());
		}
	);

	//�R���W����
	CreateSphere({}, {},50.0f);
	SetCollisionFunc(
		[&](ReferenceCollision* H, SuicideObj::CCollisionObj::SCallbackParam& p) {
			CVector3 pos = p.m_collisionPoint;
			Damage(*H, pos);
		}
	);
	GetAttributes().set(enPhysical);
	//m_col.m_collision.SetIsHighSpeed(true);//����͍����ł�	

	return true;
}

void Ari::Update() {
	//��
	if (m_hp < FLT_EPSILON) {
		Death();
	}

	//��莞�Ԃ��ƂɈړ�
	m_moveCnt++;
	if (m_moveCnt >= 60) {
		m_moveCnt = 0;

		//�e�̔���
		BulletGO* bullet = new BulletGO(
			GetPos()+ GetFront()*100.0f,
			GetFront()*(100.0f + GetFront().Dot(m_velocity)),
			this
		);
		bullet->AddComponent(std::make_unique<BD_BeamModel>());

		//�ړ���
		CVector3 govec = GetFront();// { 0.0f, 0.0f, 1.0f };
		CQuaternion rot;
		rot.SetRotation(GetLeft(), CMath::PI2*CMath::RandomZeroToOne());
		rot.Concatenate(CQuaternion(GetUp(), CMath::PI2*CMath::RandomZeroToOne()));
		rot.Multiply(govec);
		//�㏸���Â炳
		if (govec.y > 0.0f) { govec.y *= 0.9f; }
		//����
		m_velocity += govec * 20.0f*1.25f;
		//�̗̂h��
		m_shake.SetRotation(CVector3::AxisX(), 0.5f*GetFront().Dot(govec));
		m_shake.Concatenate(CQuaternion(CVector3::AxisZ(), 0.5f*GetLeft().Dot(govec)));
		//m_shake.SetRotation(CVector3(CMath::RandomZeroToOne()-0.5f, CMath::RandomZeroToOne() - 0.5f, CMath::RandomZeroToOne() - 0.5f).GetNorm(), 0.5f);
	}

	//����
	m_velocity *= 0.945f;
	m_shake.Slerp(0.1f, m_shake, CQuaternion::Identity());
	//�ړ�
	Move(m_velocity);

	//�^�[�Q�b�g�ʒu�Ɍ���
	CQuaternion rot;
	rot.MakeLookToUseXYAxis((GetPos() - m_target->GetCollisionPos()).GetNorm());
	SetRot(rot * m_shake);

	//���f���X�V
	m_model.SetPos(GetPos());
	m_model.SetRot(GetRot());
}

void Ari::Damage(const ReferenceCollision& ref, const CVector3& pos) {
	m_hp -= ref.damege;

	new CSmoke(pos, ref.direction*-1.0f * 40.0f, { 1.0f,0.0f,0.02f,1.0f });
	new CBlood(pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()), (CVector3::Up() + ref.direction*-1.0f)*50.0f);
	new CBlood(pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()), (CVector3::Up() + ref.direction*-1.0f)*50.0f);
}
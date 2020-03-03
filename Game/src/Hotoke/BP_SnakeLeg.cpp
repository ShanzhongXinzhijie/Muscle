#include "stdafx.h"
#include "BP_SnakeLeg.h"

using namespace GameObj;

namespace {
	const CVector3 hitboxOffset = CVector3::Down()*50.f;
}

void BP_SnakeLeg::InnerStart() {
	m_name = L"�X�l�[�N";

	//�A�j���[�V����
	m_animation[0].Load(L"Resource/animation/snakeleg.tka", true);

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/snakeleg.cmo", m_animation, 1);
	//�m�[�}���}�b�v�ǂݍ���
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	HRESULT hr = DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/SnakeNormalMapEX.png", nullptr, textureView.ReleaseAndGetAddressOf());
	//�}�e���A���ݒ�
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetNormalTexture(textureView.Get());
			me->SetShininess(0.45f);
		}
	);

	//����IK�ݒ�
	for (int i = 0; i < 1; i++) {
		m_ikSetting[i] = m_model->GetSkinModel().GetSkeleton().CreateIK();
		if (i == 0) {
			m_ikSetting[i]->tipBone = m_model->FindBone(L"Bone007");
			m_ikSetting[i]->rootBone = m_model->FindBone(L"Bone002");
		}
		else if (i == 1) {
			m_ikSetting[i]->tipBone = m_model->FindBone(L"Bone004");
			m_ikSetting[i]->rootBone = m_model->FindBone(L"Bone002");
		}
		else {
			m_ikSetting[i]->tipBone = m_model->FindBone(L"Bone003");
			m_ikSetting[i]->rootBone = m_model->FindBone(L"Bone002");
		}
		m_ikSetting[i]->InitFootIK();
		m_ikSetting[i]->footIKRayEndOffset = CVector3::AxisY()*-500.0f*m_ptrCore->GetScale().y;
	}

	//�����蔻��(��)
	constexpr float radius = 50.0f;
	constexpr float height = 50.0f;
	const float modelScale = m_ptrCore->GetScale().GetMax() / (0.0188f*2.0f);
	{
		//�`�󏉊���
		m_col.m_collision.CreateCapsule({}, {}, radius * modelScale, height*modelScale);
		//m_col[lr].m_collision.CreateSphere({}, {}, radius * modelScale);

		//ID�ݒ�
		m_col.m_reference.ownerID = m_ptrCore->GetFuID();
		m_col.m_reference.nonHitID = m_ptrCore->GetFuID();

		//�ʒu
		m_col.SetPos(m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()) + hitboxOffset);
		m_beforePos = m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo());

		//��������
		m_col.m_reference.attributes.set(enPhysical);

		//�Փˏ���
		m_col.m_reference.m_preCollisionFunc = [&](ReferenceCollision* H) {
			//�̓�����
			auto[damege, stunSec] = DHUtil::CalcRamDamege(m_col.m_reference.velocity, H->velocity);
			m_col.m_reference.damege = damege;
			m_col.m_reference.stunTimeSec = stunSec;
			return true;
		};
	}

	//�R���g���[���[
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_SnakeLeg(this, m_ptrCore);
	}
}

void BP_SnakeLeg::Update() {
	//�R���g���[���[�ɑ��삳����
	if (m_controller)m_controller->Update();

	//���̈ʒu�擾
	float footDistance;
	footDistance = m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()).y;

	//���Ɩ{�̂̈ʒu���߂���΁A�{�̈ʒu�����
	const float modelScale = m_ptrCore->GetScale().y / (0.0188f*2.0f);
	const float minFootDistance = 20.0f*modelScale, maxFootDistance = 167.0f*modelScale;
	//����ȏ�A���͏k�܂Ȃ�
	if (footDistance < minFootDistance) {
		//�{�̈ʒu�����
		m_ptrCore->SetPos(m_ptrCore->GetPos() + CVector3(0.0f, minFootDistance - footDistance, 0.0f));
	}
	//�����k��ł���
	if (footDistance < maxFootDistance) {
		//���X�ɑ���L�΂�
		float stretchMax = maxFootDistance - footDistance;
		m_ptrCore->AddVelocity(CVector3(0.0f, min(stretchMax, m_ptrCore->GetGravity() + 1.0f), 0.0f));
	}
}

void BP_SnakeLeg::PostUTRSUpdate() {
	//����̍X�V
	m_col.SetPos(m_ptrCore->GetPos() + hitboxOffset);
	m_col.SetVelocity(m_ptrCore->GetPos() - m_beforePos);
	m_beforePos = m_ptrCore->GetPos();

	//���̈ʒu�擾
	float footDistance;
	footDistance = m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()).y;

	const float modelScale = m_ptrCore->GetScale().y / (0.0188f*2.0f);
	const float minFootDistance = 20.0f*modelScale, maxFootDistance = 167.0f*modelScale;
	//������x�����k��ł���Ȃ�ڒn���Ă��鈵��
	if (footDistance < maxFootDistance - 1.0f) {
		//�ڒn���Ă���Ȃ��RUP
		//m_ptrCore->MulDrag(20.0f + 10.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));
		//m_ptrCore->MulRotatability(2.0f + 1.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));//��]�͂�UP
		//�U��
		m_ptrCore->SetShakePower(0.0015f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));
	}
}

void BP_SnakeLeg::Draw2D() {
}

void HCon_SnakeLeg::InnerUpdate() {
}

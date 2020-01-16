#include "stdafx.h"
#include "BP_HumanLeg.h"
#include "BulletKani.h"

using namespace GameObj;

namespace{
	const CVector3 hitboxOffset = CVector3::Down()*50.f;
}

void BP_HumanLeg::InnerStart() {
	//�A�j���[�V����(�{�[���������p)
	m_initPose.Load(L"Resource/animation/humanleg.tka", true);
	
	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/humanleg.cmo", &m_initPose, 1);
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetTranslucent(0.1f);
		}
	);

	//����IK�ݒ�
	//L
	m_ikSetting[0] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[0]->tipBone = m_model->FindBone(L"Bone022");
	m_ikSetting[0]->rootBone = m_model->FindBone(L"Bone019");
	m_ikSetting[0]->InitFootIK();
	m_ikSetting[0]->footIKRayEndOffset = CVector3::AxisY()*-2500.0f*m_ptrCore->GetScale().y;//1400
	//R
	m_ikSetting[1] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[1]->tipBone = m_model->FindBone(L"Bone022(mirrored)");
	m_ikSetting[1]->rootBone = m_model->FindBone(L"Bone019(mirrored)");
	m_ikSetting[1]->InitFootIK();
	m_ikSetting[1]->footIKRayEndOffset = CVector3::AxisY()*-2500.0f*m_ptrCore->GetScale().y;	

	//�����蔻��(��)
	constexpr float radius = 50.0f;
	constexpr float height = 50.0f;
	const float modelScale = m_ptrCore->GetScale().GetMax() / (0.0188f*2.0f);
	for (auto lr : {L,R}) {
		//�`�󏉊���
		m_col[lr].m_collision.CreateCapsule({}, {}, radius * modelScale, height*modelScale);
		//m_col[lr].m_collision.CreateSphere({}, {}, radius * modelScale);

		//ID�ݒ�
		m_col[lr].m_reference.ownerID = m_ptrCore->GetFuID();
		m_col[lr].m_reference.nonHitID = m_ptrCore->GetFuID();
		
		//�ʒu
		m_col[lr].SetPos(m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo()) + hitboxOffset);
		m_beforePos[lr] = m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo());

		//��������
		m_col[lr].m_reference.attributes.set(enPhysical);

		//�Փˏ���
		m_col[lr].m_reference.m_preCollisionFunc = [&,lr](ReferenceCollision* H) { 
			//�̓�����
			auto[damege,stunSec] = DHUtil::CalcRamDamege(m_col[lr].m_reference.velocity, H->velocity);
			m_col[lr].m_reference.damege = damege;
			m_col[lr].m_reference.stunTimeSec = stunSec;
			return true;
		};
	}

	//�R���g���[���[
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_HumanLeg(this, m_ptrCore);
	}
}

void BP_HumanLeg::Update() {
	//�R���g���[���[�ɑ��삳����
	if(m_controller)m_controller->Update();

	//���̈ʒu�擾
	float footDistance;
	footDistance = m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()).y;
	footDistance = min(footDistance, m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[1]->tipBone->GetNo()).y);

	//���Ɩ{�̂̈ʒu���߂���΁A�{�̈ʒu�����
	const float modelScale = m_ptrCore->GetScale().y / (0.0188f*2.0f);
	const float minFootDistance = 20.0f*modelScale, maxFootDistance = 167.0f*modelScale;
	//����ȏ�A���͏k�܂Ȃ�
	if (footDistance < minFootDistance) {
		//�{�̈ʒu�����
		m_ptrCore->SetPos(m_ptrCore->GetPos() + CVector3(0.0f, minFootDistance - footDistance,0.0f));
	}
	//�����k��ł���
	if (footDistance < maxFootDistance) {
		//���X�ɑ���L�΂�
		float stretchMax = maxFootDistance - footDistance;
		m_ptrCore->AddVelocity(CVector3(0.0f, min(stretchMax, m_ptrCore->GetGravity() + 1.0f), 0.0f));		
	}	
}

void BP_HumanLeg::PostUTRSUpdate() {
	//����̍X�V
	for (auto lr : LR) {
		m_col[lr].SetPos(m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo()) + hitboxOffset);
		m_col[lr].SetVelocity(m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo()) - m_beforePos[lr]);
		m_beforePos[lr] = m_model->GetBonePos(m_ikSetting[lr]->tipBone->GetNo());
	}

	//���̈ʒu�擾
	float footDistance;
	footDistance = m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[0]->tipBone->GetNo()).y;
	footDistance = min(footDistance, m_ptrCore->GetPos().y - m_model->GetBonePos(m_ikSetting[1]->tipBone->GetNo()).y);
	
	const float modelScale = m_ptrCore->GetScale().y / (0.0188f*2.0f);
	const float minFootDistance = 20.0f*modelScale, maxFootDistance = 167.0f*modelScale;
	//������x�����k��ł���Ȃ�ڒn���Ă��鈵��
	if (footDistance < maxFootDistance - 1.0f) {
		//�ڒn���Ă���Ȃ��RUP
		m_ptrCore->MulDrag(20.0f + 10.0f*max(0.0f,-m_ptrCore->GetTotalVelocity().y));
		m_ptrCore->MulRotatability(2.0f+ 1.0f*max(0.0f, -m_ptrCore->GetTotalVelocity().y));//��]�͂�UP
		//�W�����v
		if (m_isJump) {
			float power = 25.0f + 38.0f*(1.0f - abs((minFootDistance - footDistance) / (minFootDistance - maxFootDistance)));
			m_ptrCore->SetMaxLinearVelocity(CVector3::Up()*power);
		}
	}
	
	m_isJump = false;
}

void BP_HumanLeg::PostLoopUpdate() {
	//if (CanKick()) {
	//	DrawQuad2D({ 0.5f - 0.2f,0.4f - 0.025f,0.0f }, { 0.5f + 0.25f,0.4f + 0.025f,0.0f }, {1.f,1.f,1.f,0.1f}, m_ptrCore->GetPlayerNum());
	//}
}

void BP_HumanLeg::Draw2D() {
	//���܂�!
	if (CanKick()) {
		CVector2 size = m_ptrCore->GetJapaneseFont()->GetScale();
		CVector4 color = m_ptrCore->GetJapaneseFont()->GetColor();

		m_ptrCore->GetJapaneseFont()->SetScale(size*1.5f);
		m_ptrCore->GetJapaneseFont()->SetColor(m_ptrCore->GetWarningFont()->GetColor());

		m_ptrCore->GetJapaneseFont()->Draw(L"[LT]or[RT]�L�b�N!", { 0.5f,0.4f }, { 0.5f,0.5f });

		m_ptrCore->GetJapaneseFont()->SetColor(color);
		m_ptrCore->GetJapaneseFont()->SetScale(size);
	}

	//����
	m_ptrCore->GetJapaneseFont()->DrawFormat(
		m_leftStomp > 0 ? L"�̂���ӂ݂�:%d����" : L"�����Ȃ�",
		{ 0.3f,0.95f + 0.01f }, { 0.0f,0.0f },
		m_leftStomp
	);
}

bool BP_HumanLeg::CanKick()const {
	//�L�b�N�\������
	constexpr float KICK_LENGE_Sq = CMath::Square(200.0f*METER);
	CVector3 distance = m_ptrCore->GetTargetPos() - m_ptrCore->GetPos(); 
	float rad = CVector3::AngleOf2NormalizeVector((m_ptrCore->GetVanisingPoint() - m_ptrCore->GetPos()).GetNorm(), distance.GetNorm());
	distance.y = 0.0f;
	return m_ptrCore->GetTarget() && m_leftStomp > 0 && distance.LengthSq() < KICK_LENGE_Sq && rad < CMath::DegToRad(67.0f);
}

void BP_HumanLeg::Jump() {
	m_isJump = true;
}

void BP_HumanLeg::Stomp() {
	if (!CanKick()) { return; }

	//���݂��쐬
	BulletGO* bullet = new BulletGO(
		m_ptrCore->GetTargetPos(),//+ CVector3::Up()*150.0f,
		CVector3::Down()*5.0f,
		m_ptrCore,
		3.0f
	);
	bullet->m_isDamegeHeightBoost = false;//�_���[�W�u�[�X�g����
	bullet->SetStunTimeSec(0.5f);//�X�^������
	bullet->AddComponent(std::make_unique<BD_LegModel>(m_ptrCore->GetRot(), m_ptrCore->GetScale()));//���f��
	bullet->AddComponent(std::make_unique<BD_OneContactMask>());//���i�q�b�g���Ȃ�

	m_leftStomp--;
}

void HCon_HumanLeg::InnerUpdate() {
	if (m_ptrCore->GetPad()->GetLegDown()) {
		m_ptrBody->Jump();
	}
	//if (m_ptrCore->GetPad()->GetDoubleLegDown()) {//��������
	if (m_ptrCore->GetPad()->GetLegDown()) {
		m_ptrBody->Stomp();
	}
}
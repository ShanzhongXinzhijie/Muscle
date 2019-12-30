#include "stdafx.h"
#include "FlyingGrass.h"

using namespace GameObj;

FlyingGrass::FlyingGrass(const CVector3& pos, const CVector3& move, int time) : m_maxLifeTime(time)
{
	//���f���ǂݍ���
	std::unique_ptr<CInstancingModelRender> model = std::make_unique<CInstancingModelRender>();	
	bool isType2 = CMath::RandomZeroToOne() > 0.5f;
	if (isType2) {
		model->Init(MAX_NUM, L"Resource/modelData/pinGrass2.cmo", nullptr, 0, enFbxUpAxisY);
	}
	else {
		model->Init(MAX_NUM, L"Resource/modelData/pinGrass.cmo", nullptr, 0, enFbxUpAxisY);
	}
	model->GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			me->SetShininess(0.4f);
		}
	);

	//�X�e�[�^�X
	model->SetPos(pos);
	CQuaternion rot(CVector3::AxisX(), -CMath::PI_HALF*0.9f + CMath::PI_HALF*0.9f*2.0f*CMath::RandomZeroToOne());
	rot.Concatenate(CQuaternion(CVector3::AxisY(), -CMath::PI2 + CMath::PI2*2.0f*CMath::RandomZeroToOne()));
	model->SetRot(rot);
	model->SetScale((CMath::RandomZeroToOne()*0.0015f + 0.003f)*(isType2 ? 1.5f : 1.0f));
	
	//�ړ�
	SetMove(move);
	CVector3 axis = { (1.0f - CMath::RandomZeroToOne()*2.0f) ,(1.0f - CMath::RandomZeroToOne()*2.0f) ,(1.0f - CMath::RandomZeroToOne()*2.0f) };
	axis.Normalize();
	SetRotation({ axis, CMath::DegToRad(1.0f) * (1.0f - CMath::RandomZeroToOne()*2.0f) });

	//�p�[�e�B�N���Đ�
	Play(std::move(model), m_maxLifeTime);
}
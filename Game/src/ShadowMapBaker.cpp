#include "stdafx.h"
#include "ShadowMapBaker.h"

void ShadowMapBaker::AddDrawModel(GameObj::CSkinModelRender& caster, bool isCasterSetUnable) {
	//���f���ǉ�
	GetGraphicsEngine().GetShadowMapRender().AddDrawModel(&caster.GetSkinModel(), caster.GetDrawPriority(), caster.GetIsShadowDrawReverse());
	if (isCasterSetUnable) {
		//���f���L���X�^�[�I�t
		caster.SetIsShadowCaster(false);
	}
}

void ShadowMapBaker::Bake(int x_size, int y_size, const CVector3& lightDir, const CVector3& area, const CVector3& forcusPoint) {
	//GPU�C�x���g�̊J�n
	GetGraphicsEngine().BeginGPUEvent(L"ShadowMapBaker::Bake");

	//������
	m_isBaked = false;
	m_shadowMap.Init(x_size, y_size, lightDir);
	m_shadowMap.SetArea(area);//�V���h�E�}�b�v�͈̔�(Z�����C�g�̕���)
	m_shadowMap.SetTarget(forcusPoint);//�V���h�E�}�b�v�͈̔͂̒��S�ʒu

	//�V���h�E�}�b�v���x�C�N
	GetGraphicsEngine().GetShadowMapRender().Render();
	GetGraphicsEngine().GetShadowMapRender().PostRender();

	//�Ȍ�A�X�V���Ȃ��V���h�E�}�b�v�ɂ���	
	m_shadowMap.SetIsUpdate(false);

	m_isBaked = true;

	//GPU�C�x���g�̏I��
	GetGraphicsEngine().EndGPUEvent();
}
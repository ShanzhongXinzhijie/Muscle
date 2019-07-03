#include "stdafx.h"
#include "BP_KaniArm.h"

using namespace GameObj;

void BP_KaniArm::InnerStart() {
	//�A�j���[�V����(�{�[���������p)
	m_initPose.Load(L"Resource/animation/kaniarm.tka", true);

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/kaniarm.cmo", &m_initPose, 1);

	//�r��IK�ݒ�
	/*setting = m_model[4].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031");
	setting->rootBone = m_model[4].FindBone(L"Bone026");

	setting = m_model[4].GetSkinModel().GetSkeleton().CreateIK();
	setting->tipBone = m_model[4].FindBone(L"Bone031(mirrored)");
	setting->rootBone = m_model[4].FindBone(L"Bone026(mirrored)");*/

	//�G�t�F�N�g
	m_muzzleFlash.Init(L"Resource/spriteData/kaniFlare.png");// , 1, false);
	if(m_muzzleFlash.GetIsInstancing()){
		D3D_SHADER_MACRO macros[] = { "INSTANCING", "1", "ALL_VS", "1", NULL, NULL };
		m_vs[0].Load("Preset/shader/model.fx", "VSMain_RenderZ", Shader::EnType::VS, "INSTANCING", macros);
		m_vs[1].Load("Preset/shader/model.fx", "VSMainSkin_RenderZ", Shader::EnType::VS, "INSTANCING", macros);
	}
	else {
		m_vs[0].Load("Preset/shader/model.fx", "VSMain_RenderZ", Shader::EnType::VS);
		m_vs[1].Load("Preset/shader/model.fx", "VSMainSkin_RenderZ", Shader::EnType::VS);
	}
	{
		D3D_SHADER_MACRO macros[] = { "TEXTURE", "1", NULL, NULL };
		m_ps.Load("Preset/shader/model.fx", "PSMain_SozaiNoAzi_ConvertToPMA", Shader::EnType::PS, "TEXTURE", macros);
	}
	m_muzzleFlash.GetModel().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			if (mat->GetModelEffect()->GetIsSkining()) {
				//�X�L�����f��
				mat->SetVS(&m_vs[1]);
			}
			else {
				//�X�L������Ȃ����f��
				mat->SetVS(&m_vs[0]);
			}
			//�s�N�Z���V�F�[�_
			mat->SetPS(&m_ps);
		}
	);
	m_muzzleFlash.GetModel().SetIsPostDraw(true, PostDrawModelRender::enAdd);

}

void BP_KaniArm::Update() {
	m_muzzleFlash.SetPos(m_ptrCore->GetPos());
	m_muzzleFlash.SetScale({ 1000.0f, 500.0f,1000.0f });
}
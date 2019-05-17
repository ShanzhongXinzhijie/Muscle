#include "stdafx.h"
#include "Shibuya.h"


Shibuya::Shibuya()
{
	//���C�g�쐬
	m_directionLight.SetDirection(CVector3::AxisZ()*-1.0f);
	m_directionLight.SetColor(CVector3::One() * 0.5f);

	//�X���f��
	m_model.Init(L"Resource/modelData/tikei.cmo");//city
	//m_model.GetSkinModel().FindMaterialSetting([](MaterialSetting* mat) {
		//mat->SetLightingEnable(false);
	//});
	m_model.SetScale(CVector3::One()*50.0f);
	m_phyStaticObject.CreateMesh(m_model);

	//�V�F�[�_
	D3D_SHADER_MACRO macros[] = { "MOTIONBLUR", "1", "NORMAL_MAP", "1", NULL, NULL };
	m_ps.Load("Preset/shader/TriPlanarMapping.fx", "PS_TriPlanarMapping", Shader::EnType::PS, "MOTIONBLUR,NORMAL_MAP", macros);
	//�m�[�}���}�b�v
	ID3D11ShaderResourceView* tex = nullptr;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/spriteData/n_land.dds", nullptr, &tex);
	DW_ERRORBOX(FAILED(hr), "�n�`�̃m�[�}���}�b�v�ǂݍ��݂Ɏ��s");
	//���f���ɃV�F�[�_�ƃm�[�}���}�b�v�ݒ�
	m_model.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(tex);
			mat->SetPS(&m_ps);
		}
	);
	//�m�[�}���}�b�v�A�����[�X
	if (tex) {
		tex->Release();
	}

	//��
	m_sky.Init(L"Resource/cubemap/cube2.dds");

	//�؁X
	//float area_min = -8000.0f - 500.0f, area_max = -8000.0f + 500.0f;
	//m_treeGene.Generate({ area_min,-70.0f*500.0f,-500.0f }, { area_max,70.0f*50.0f,500.0f }, 500);
	m_treeGene.Generate({ -70.0f*50.0f*0.1f,-70.0f*50.0f,-70.0f*50.0f*0.1f }, { 70.0f*50.0f*0.1f,70.0f*50.0f,70.0f*50.0f*0.1f }, 500);

	//�V���h�E�}�b�v
	//m_shadowmap.Init(3,//������
	//	m_directionLight.GetDirection(),//���C�g�̕���
	//	1.0f//�V���h�E�}�b�v�͈̔�(���C���J������Far�ɂ�����W���ł�)
	//);
	//m_shadowmap.SetNear(50.0f);
	//m_shadowmap.SetFar(20000.0f);
}

Shibuya::~Shibuya()
{
}

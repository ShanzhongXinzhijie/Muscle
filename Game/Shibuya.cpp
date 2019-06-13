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

	//�m�[�}���}�b�v
	ID3D11ShaderResourceView* tex = nullptr, *normaltex = nullptr;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/spriteData/n_land.dds", nullptr, &normaltex);
	hr = DirectX::CreateDDSTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/spriteData/land.dds", nullptr, &tex);
	DW_ERRORBOX(FAILED(hr), "�n�`�̃m�[�}���}�b�v�ǂݍ��݂Ɏ��s");
	
	//���f���ɃV�F�[�_�ƃm�[�}���}�b�v�ݒ�
	m_model.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(normaltex);
			mat->SetAlbedoTexture(tex);
			mat->SetTriPlanarMappingPS();
			//mat->SetTriPlanarMappingUVScale(0.01f);
		}
	);

	//�m�[�}���}�b�v�A�����[�X
	if (tex) {
		tex->Release();
	}
	normaltex->Release();

	m_knight.Init(L"Resource/modelData/knight.cmo");
	m_knight.SetPos(CVector3::AxisY()*850.0f);
	m_knight.SetScale(0.4f);
	//m_knight.GetSkinModel().FindMaterial([](ModelEffect* mat) {mat->SetEmissive(4.0f); });
	
	m_dinosaur.Init(L"Resource/modelData/dinosaur.cmo");
	m_dinosaur.SetPos(CVector3::AxisY()*900.0f+ CVector3::AxisX()*50.0f);
	m_dinosaur.SetScale(CVector3::One()*0.09f);

	//��
	m_sky.Init(L"Resource/cubemap/cube2.dds");

	//�؁X
	//float area_min = -8000.0f - 500.0f, area_max = -8000.0f + 500.0f;
	//m_treeGene.Generate({ area_min,-70.0f*500.0f,-500.0f }, { area_max,70.0f*50.0f,500.0f }, 500);
	m_treeGene.Generate({ -70.0f*50.0f,-70.0f*50.0f,-70.0f*50.0f }, { 70.0f*50.0f,70.0f*50.0f,70.0f*50.0f }, 4000);

	//�V���h�E�}�b�v
	//m_shadowmap.Init(3,//������
	//	m_directionLight.GetDirection(),//���C�g�̕���
	//	1.0f//�V���h�E�}�b�v�͈̔�(���C���J������Far�ɂ�����W���ł�)
	//);
	//m_shadowmap.SetNear(50.0f);
	//m_shadowmap.SetFar(20000.0f);

	//�r���{�e�X�g
	m_billboard.Init(L"Resource/spriteData/test.png", 256);
	m_billboard.SetPos(CVector3::Up()*1000.0f);
	m_billboard.SetScale(100.0f);

	m_billboard2.Init(L"Resource/spriteData/test.png",256);
	m_billboard2.SetPos(CVector3::Up()*1200.0f);
	m_billboard2.SetScale(50.0f);

	m_imp.Init(L"Resource/modelData/knight.cmo", { 2048*2,2048*2 }, { 9,9 },256);
	m_imp.SetPos(CVector3::Up()*1500.0f+ CVector3::AxisX()*300.0f);
	m_imp.SetScale(0.4f*10.0f);

	m_imp2.Init(L"Resource/modelData/knight.cmo", { 2048 * 2,2048 * 2 }, { 9,9 });
	m_imp2.SetPos(CVector3::Up()*1000.0f + CVector3::AxisX()*300.0f);
	m_imp2.SetScale(0.4f*10.0f);

	m_knight.SetPos(CVector3::AxisY()*1500.0f);
	m_knight.SetScale(0.4f*10.0f);
}

Shibuya::~Shibuya()
{
}

void Shibuya::PostLoopUpdate() {
	//��
	DrawLine(CVector3::Zero(), CVector3::AxisX()*1000.0f, { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(CVector3::Zero(), CVector3::AxisY()*1000.0f, { 0.0f,1.0f,0.0f,1.0f });
	DrawLine(CVector3::Zero(), CVector3::AxisZ()*1000.0f, { 0.0f,0.0f,1.0f,1.0f });
}
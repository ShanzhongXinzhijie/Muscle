#include "stdafx.h"
#include "Shibuya.h"


Shibuya::Shibuya()
{
	//ライト作成
	m_directionLight.SetDirection(CVector3::AxisZ()*-1.0f);
	m_directionLight.SetColor(CVector3::One() * 0.5f);

	//街モデル
	m_model.Init(L"Resource/modelData/tikei.cmo");//city
	//m_model.GetSkinModel().FindMaterialSetting([](MaterialSetting* mat) {
		//mat->SetLightingEnable(false);
	//});
	m_model.SetScale(CVector3::One()*50.0f);
	m_phyStaticObject.CreateMesh(m_model);

	//シェーダ
	D3D_SHADER_MACRO macros[] = { "MOTIONBLUR", "1", "NORMAL_MAP", "1", NULL, NULL };
	m_ps.Load("Preset/shader/TriPlanarMapping.fx", "PS_TriPlanarMapping", Shader::EnType::PS, "MOTIONBLUR,NORMAL_MAP", macros);
	//ノーマルマップ
	ID3D11ShaderResourceView* tex = nullptr;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/spriteData/n_land.dds", nullptr, &tex);
	DW_ERRORBOX(FAILED(hr), "地形のノーマルマップ読み込みに失敗");
	//モデルにシェーダとノーマルマップ設定
	m_model.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(tex);
			mat->SetPS(&m_ps);
		}
	);
	//ノーマルマップ、リリース
	if (tex) {
		tex->Release();
	}

	//空
	m_sky.Init(L"Resource/cubemap/cube2.dds");

	//木々
	//float area_min = -8000.0f - 500.0f, area_max = -8000.0f + 500.0f;
	//m_treeGene.Generate({ area_min,-70.0f*500.0f,-500.0f }, { area_max,70.0f*50.0f,500.0f }, 500);
	m_treeGene.Generate({ -70.0f*50.0f*0.1f,-70.0f*50.0f,-70.0f*50.0f*0.1f }, { 70.0f*50.0f*0.1f,70.0f*50.0f,70.0f*50.0f*0.1f }, 500);

	//シャドウマップ
	//m_shadowmap.Init(3,//分割数
	//	m_directionLight.GetDirection(),//ライトの方向
	//	1.0f//シャドウマップの範囲(メインカメラのFarにかかる係数です)
	//);
	//m_shadowmap.SetNear(50.0f);
	//m_shadowmap.SetFar(20000.0f);
}

Shibuya::~Shibuya()
{
}

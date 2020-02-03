#pragma once

#include"TreeGene.h"
#include"StageObjectGenerator.h"
#include"CDeathHotoke.h"
#include"ShadowMapBaker.h"

class Shibuya : IGameObject
{
public:
	Shibuya();

	//void Update()override;
	void PostLoopUpdate()override;
	//void PostRender()override;

private:
	IFu m_graund;
	GameObj::CSkinModelRender m_graundModel;
	//GameObj::CSkinModelRender m_areaWallModel;

	GameObj::CSkybox m_sky;
	GameObj::CSkinModelRender m_midGraund, m_midCastle, m_farGraund;

	GameObj::CDirectionLight m_directionLight;
	//CascadeShadowHandler m_shadowmap;
	
	StageObjectGenerator m_objGene;
	//GameObj::CSkinModelRender m_dinosaur, m_skyModel, m_uni;
	//GameObj::CSkinModelRender m_knight;// , m_cloud[6];
	//SkinModelEffectShader m_psCloud;
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cloudtex;
	//float m_cloudTimer = 0.0f;

	//CBillboard m_billboard, m_billboard2;
	//CImposter m_imp, m_imp2;

	//HUDFont m_font;
	//float m_shinnes = 0.3f;

	//ShadowMapBaker m_shadowMapBaker[4];

	//CDeathHotoke m_hotoke;
};


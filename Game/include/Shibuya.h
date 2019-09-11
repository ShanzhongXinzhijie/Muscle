#pragma once

#include"TreeGene.h"
#include"StageObjectGenerator.h"
#include"CDeathHotoke.h"
#include"ShadowMapBaker.h"

class Shibuya : IGameObject
{
public:
	Shibuya();

	void Update()override;
	void PostLoopUpdate()override;

private:
	GameObj::CSkinModelRender m_model;
	GameObj::CSkybox m_sky;
	GameObj::CDirectionLight m_directionLight;
	CascadeShadowHandler m_shadowmap;

	PhysicsStaticObject m_phyStaticObject;

	StageObjectGenerator m_objGene;
	GameObj::CSkinModelRender m_knight, m_dinosaur, m_skyModel, m_cloud[6], m_uni;
	SkinModelEffectShader m_psCloud;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cloudtex;
	float m_cloudTimer = 0.0f;

	CBillboard m_billboard, m_billboard2;
	CImposter m_imp, m_imp2;

	ShadowMapBaker m_shadowMapBaker[4];

	//CDeathHotoke m_hotoke;
};


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
	//void PostRender()override;

private:
	IFu m_graund;
	GameObj::CSkinModelRender m_model;
	//SuicideObj::CCollisionObj m_graundCollision;
	//PhysicsStaticObject m_phyStaticObject;

	GameObj::CSkybox m_sky;
	GameObj::CDirectionLight m_directionLight;
	//CascadeShadowHandler m_shadowmap;
	
	StageObjectGenerator m_objGene;
	GameObj::CSkinModelRender m_knight, m_dinosaur, m_skyModel, m_cloud[6], m_uni;
	SkinModelEffectShader m_psCloud;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_cloudtex;
	float m_cloudTimer = 0.0f;

	CBillboard m_billboard, m_billboard2;
	CImposter m_imp, m_imp2;

	//HUDFont m_font;
	//float m_shinnes = 0.3f;

	//ShadowMapBaker m_shadowMapBaker[4];

	CDeathHotoke m_hotoke;
};


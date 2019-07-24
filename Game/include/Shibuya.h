#pragma once

#include"TreeGene.h"
#include"StageObjectGenerator.h"
#include"CDeathHotoke.h"
#include"ShadowMapBaker.h"

class Shibuya : IGameObject
{
public:
	Shibuya();

	void PostLoopUpdate()override;

private:
	GameObj::CSkinModelRender m_model;
	GameObj::CSkybox m_sky;
	GameObj::CDirectionLight m_directionLight;
	CascadeShadowHandler m_shadowmap;

	PhysicsStaticObject m_phyStaticObject;

	StageObjectGenerator m_objGene;
	GameObj::CSkinModelRender m_knight, m_dinosaur;
	CBillboard m_billboard, m_billboard2;
	CImposter m_imp, m_imp2;

	ShadowMapBaker m_shadowMapBaker[4];

	CDeathHotoke m_hotoke;
};


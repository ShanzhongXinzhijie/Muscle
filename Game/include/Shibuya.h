#pragma once

#include"TreeGene.h"
#include"CDeathHotoke.h"

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

	TreeGene m_treeGene;
	GameObj::CSkinModelRender m_knight, m_dinosaur;
	CBillboard m_billboard, m_billboard2;
	CImposter m_imp, m_imp2;

	//CDeathHotoke m_hotoke;
};


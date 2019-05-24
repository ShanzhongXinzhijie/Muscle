#pragma once

#include"TreeGene.h"

class Shibuya : IGameObject
{
public:
	Shibuya();
	~Shibuya();

	void Update()override;
	void PostLoopUpdate()override;

private:
	GameObj::CSkinModelRender m_model;
	GameObj::CSkybox m_sky;
	GameObj::CDirectionLight m_directionLight;
	CascadeShadowHandler m_shadowmap;

	PhysicsStaticObject m_phyStaticObject;

	TreeGene m_treeGene;
	GameObj::CSkinModelRender m_knight, m_dinosaur;
	GameObj::CBillboard m_billboard;
	GameObj::CImposter m_imp;
};


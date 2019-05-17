#pragma once

#include"TreeGene.h"

class Shibuya
{
public:
	Shibuya();
	~Shibuya();
private:
	GameObj::CSkinModelRender m_model;
	GameObj::CSkybox m_sky;
	GameObj::CDirectionLight m_directionLight;
	CascadeShadowHandler m_shadowmap;

	PhysicsStaticObject m_phyStaticObject;

	TreeGene m_treeGene;
};


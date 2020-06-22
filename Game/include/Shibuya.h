#pragma once

#include"TreeGene.h"
#include"StageObjectGenerator.h"
#include"CDeathHotoke.h"
#include"ShadowMapBaker.h"

class Shibuya// : IGameObject
{
public:
	Shibuya(bool isTreeGene);
	//~Shibuya() {
	//	g_treeRunner.Disable();
	//}

	//void PostLoopUpdate()override;

private:
	IFu m_graund;
	GameObj::CSkinModelRender m_graundModel;

	GameObj::CSkinModelRender m_midGraund, m_midCastle, m_farGraund;
	GameObj::CSkybox m_sky;

	GameObj::CDirectionLight m_directionLight;
	
	StageObjectGenerator m_objGene;
	StageObjectGenerator m_towerGene;
	StageObjectGenerator m_helicoGene;
	CHuman m_human;

	//CascadeShadowHandler m_shadowmap;
	//ShadowMapBaker m_shadowMapBaker[4];
};


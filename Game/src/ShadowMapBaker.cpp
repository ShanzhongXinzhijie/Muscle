#include "stdafx.h"
#include "ShadowMapBaker.h"

void ShadowMapBaker::AddDrawModel(GameObj::CSkinModelRender& caster, bool isCasterSetUnable) {
	//モデル追加
	GetGraphicsEngine().GetShadowMapRender().AddDrawModel(&caster.GetSkinModel(), caster.GetDrawPriority(), caster.GetIsShadowDrawReverse());
	if (isCasterSetUnable) {
		//モデルキャスターオフ
		caster.SetIsShadowCaster(false);
	}
}

void ShadowMapBaker::Bake(int x_size, int y_size, const CVector3& lightDir, const CVector3& area, const CVector3& forcusPoint) {
	//GPUイベントの開始
	GetGraphicsEngine().BeginGPUEvent(L"ShadowMapBaker::Bake");

	//初期化
	m_isBaked = false;
	m_shadowMap.Init(x_size, y_size, lightDir);
	m_shadowMap.SetArea(area);//シャドウマップの範囲(Zがライトの方向)
	m_shadowMap.SetTarget(forcusPoint);//シャドウマップの範囲の中心位置

	//シャドウマップをベイク
	GetGraphicsEngine().GetShadowMapRender().Render();
	GetGraphicsEngine().GetShadowMapRender().PostRender();

	//以後、更新しないシャドウマップにする	
	m_shadowMap.SetIsUpdate(false);

	m_isBaked = true;

	//GPUイベントの終了
	GetGraphicsEngine().EndGPUEvent();
}
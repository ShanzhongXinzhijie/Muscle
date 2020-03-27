#include "stdafx.h"
#include "BloodPoint.h"


BloodPoint::BloodPoint(
	const CVector3& pos, const CVector3& normal, float scale
)
{
	//ビルボード読み込み
	m_model.Init(L"Resource/spriteData/BloodDrop31.png", MAX_NUM, false);
	m_model.GetModel().InitPostDraw(PostDrawModelRender::enAlpha, false, GameObj::CSkinModelRender::enRevSoftParticle);//ポストドロー(逆ソフトパーティクル)
	m_model.GetModel().SetIsShadowCaster(false);

	//ステータス
	m_model.SetPos(pos);
	m_model.SetScale(scale);
	CQuaternion rot;
	rot.MakeLookToUseXYAxis(normal);
	rot.Multiply({ CVector3::AxisZ(),CMath::PI2*CMath::RandomZeroToOne() });
	m_model.SetRot(rot);
}

void BloodPoint::Update() {
	m_lifeTime--;
	if (m_lifeTime <= 0) {
		delete this;
		return;
	}
}
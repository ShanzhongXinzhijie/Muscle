#include "stdafx.h"
#include "CBlood.h"

bool CBlood::Start() {
	//モデル
	m_model.Init(L"BLOOD",false);
	m_model.SetPos(m_posOld, GetPos());
	if (m_radiusSetting > 0.0f) {
		m_model.SetRadius(m_radiusSetting);
	}
	else {
		m_model.SetRadius(0.5f + CMath::RandomZeroToOne());
	}

	//移動方向ランダム
	float speed = m_vector.Length(); 
	m_vector.Normalize();
	m_vector += CVector3(0.4f - 0.8f*CMath::RandomZeroToOne(), 0.4f - 0.8f*CMath::RandomZeroToOne(), 0.4f - 0.8f*CMath::RandomZeroToOne());
	m_vector.Normalize();
	m_vector *= speed;

	//長さ
	m_lengthScale = 0.1f + 1.2f*CMath::RandomZeroToOne();
	m_lengthScale = max(1.0f, m_lengthScale);
	//落下速度
	m_gravityScale = 0.5f + 1.0f*CMath::RandomZeroToOne();

	return true;
}

void CBlood::Update() {
	//移動
	m_posOld = GetPos();
	Move(m_vector);

	//移動方向変化
	m_vector *= CVector3(1.2f - 0.4f*CMath::RandomZeroToOne(), 1.2f - 0.4f*CMath::RandomZeroToOne(), 1.2f - 0.4f*CMath::RandomZeroToOne());
	m_vector.y -= 7.0f * m_gravityScale;

	//寿命処理
	m_lifeTime -= FRAME_PER_SECOND;
	if (m_lifeTime < FLT_EPSILON) {
		delete this; return;
	}

	//当たり判定
	if (m_createCollision) {
		//レイで判定
		btVector3 rayStart = m_posOld;
		btVector3 rayEnd = GetPos();
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			//跳ね返り作成
			for (int i = 0; i < 2; i++) {
				new CBlood(
					//位置
					gnd_ray.m_hitPointWorld,
					//移動
					gnd_ray.m_hitNormalWorld * (m_vector.Length()*0.5f),
					//半径
					m_model.GetRadius()*0.5f + (m_model.GetRadius()*0.2f*CMath::RandomZeroToOne()),
					//判定作らない
					false
				);
			}
			//TODO 血溜まり
			//TODO
			//逆ソフトパーティクルの血痕
		}
	}

	//モデル更新
	m_model.SetPos(GetPos() + (m_posOld - GetPos())*m_lengthScale, GetPos());
}
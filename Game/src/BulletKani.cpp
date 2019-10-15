#include "stdafx.h"
#include "BulletKani.h"

BulletGO::BulletGO(IBulletDecolator* ptrDecolator, const CVector3& pos, const CVector3& move)
	: m_decolator(ptrDecolator), m_pos(pos), m_posOld(pos), m_vector(move)
{
	//初期化
	if(m_decolator)m_decolator->SetBullet(this);

	//モデル
	m_model.Init(L"BLUE");
	m_model.SetPos(m_posOld, m_pos);
	m_model.SetRadius(6.0f);
	m_lastDrawPos = m_pos;
	
	//当たり判定
	m_col.m_collision.CreateSphere(m_pos, {}, 3.0f);
	m_col.m_reference.damege = 1.0f;
	m_col.m_reference.attributes.set(enPhysical);
	m_col.m_reference.attributes.set(enFlame);
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//クラス取り出す
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				if (H->attributes[enPhysical]) {
					//物理属性なら死
					m_lifeTime = 0.0f;
				}
			}
			if (!p.m_isCCollisionObj) {
				//相手がCCollisionObjじゃなくても死
				m_lifeTime = 0.0f;
			}
		}
	);
	m_col.m_collision.SetIsCollisionStaticObject(true);//静的オブジェクトとも衝突する
	m_col.m_collision.SetIsHighSpeed(true);//これは高速です
}

void BulletGO::Update() {
	//m_isMoved = true;

	//移動
	m_posOld = m_pos;
	m_pos += m_vector;

	//寿命処理
	m_lifeTime -= GetDeltaTimeSec();
	if (m_lifeTime < FLT_EPSILON) {
		delete this; return;
	}

	//コリジョン更新
	m_col.SetPos(m_pos);
	m_col.SetDir(m_vector.GetNorm());
}

void BulletGO::PostLoopUpdate() {
	//モデル更新
	if (!m_isMoved) {
		m_model.SetPos(m_posOld, m_pos); m_isMoved = true;
	}
	else {
		m_model.Move(m_pos - m_lastDrawPos);
	}
	m_lastDrawPos = m_pos;
}
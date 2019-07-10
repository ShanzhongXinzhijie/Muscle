#include "stdafx.h"
#include "BulletKani.h"

BulletKani::BulletKani(const CVector3& pos, const CVector3& move) : m_pos(pos), m_posOld(pos), m_vector(move) {
	m_model.Init();
	m_model.SetPos(m_posOld, m_pos);
	m_model.SetRadius(3.0f);
}

void BulletKani::Update() {
	//移動
	m_posOld = m_pos;
	m_pos += m_vector;

	//寿命処理
	m_lifeTime -= GetDeltaTimeSec();
	if (m_lifeTime < FLT_EPSILON) {
		delete this; return;
	}

	//モデル更新
	m_model.Move(m_vector);
}
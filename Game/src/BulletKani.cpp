#include "stdafx.h"
#include "BulletKani.h"

bool BulletKani::Start() {
	//m_model.Init(m_sInstancingMax, L"Resource/modelData/tree_tall.cmo");

	return true;
}

void BulletKani::Update() {
	m_posOld = m_pos;
	m_pos += m_vector;

	m_lifeTime -= GetDeltaTimeSec();
	if (m_lifeTime < FLT_EPSILON) { delete this; return; }
}

void BulletKani::PostLoopUpdate() {
	DrawLine(m_pos, m_posOld, { 1.0f,1.0f ,1.0f ,1.0f });
}
#pragma once
#include"BeamModel.h"

class BulletKani :
	public IGameObject
{
public:
	BulletKani(const CVector3& pos, const CVector3& move);

	void Update()override;

private:
	BeamModel m_model;
	float m_lifeTime = 3.0f;
	CVector3 m_pos, m_posOld, m_vector;

	//コリジョン
	DHCollision m_col;
};

/// <summary>
/// バレットに
/// ホーミング・特殊軌道
/// 接触処理・爆発
/// ↑この辺をデコるためのやつ
/// </summary>
class IBulletDecolator {
public:
	void Update() {
		m_ptrBullet->Update();
		InnerUpdate();
	}
	void Contact() {
		m_ptrBullet->Contact();
		InnerContact();
	}

	virtual void InnerUpdate() = 0;
	virtual void InnerContact() = 0;

private:
	IBulletDecolator* m_ptrBullet = nullptr;
};

/// <summary>
/// バレットクラス
/// </summary>
class IBullet : public IGameObject {
public:

private:
	//IBulletModel*;
	IBulletDecolator* m_decolator = nullptr;

	float m_lifeTime = 3.0f;
	CVector3 m_pos, m_posOld, m_vector;
};

//TODO ムラ(MHW) 地形ノイズ
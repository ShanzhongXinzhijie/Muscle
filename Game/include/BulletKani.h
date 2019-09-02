#pragma once
#include"BeamModel.h"

class BulletGO;

/// <summary>
/// バレットに
/// ホーミング・特殊軌道
/// 接触処理・爆発
/// ↑この辺をデコるためのやつ
/// </summary>
class IBulletDecolator {
public:
	IBulletDecolator(IBulletDecolator* ptrDecolator) :m_decolator(ptrDecolator) {}

	void DecoUpdate() {
		m_decolator->DecoUpdate();
		InnerUpdate();
	}
	void DecoPostUpdate() {
		m_decolator->DecoPostUpdate();
		InnerPostUpdate();
	}
	void DecoContact() {
		m_decolator->DecoContact();
		InnerContact();
	}

	virtual void InnerUpdate() = 0;
	virtual void InnerPostUpdate() = 0;
	virtual void InnerContact() = 0;

	void SetBullet(BulletGO* bullet) {
		m_bullet = bullet;
		if (m_decolator) {
			m_decolator->SetBullet(m_bullet);
		}
	}
	BulletGO* GetBullet() {
		//if (m_decolator) {
		//	return m_decolator->GetBullet();
		//}
		//else {
			return m_bullet;
		//}
	}

private:
	IBulletDecolator* m_decolator = nullptr;
	BulletGO* m_bullet = nullptr;
};

/// <summary>
/// バレットを動かすクラス
/// </summary>
class BulletGO : public IQSGameObject{
public:
	BulletGO(IBulletDecolator* ptrDecolator, const CVector3& pos, const CVector3& move);

	void Update()override;
	void PostLoopUpdate()override;

private:
	//デコレーター
	IBulletDecolator* m_decolator = nullptr;

	//モデル
	BeamModel m_model;
	bool m_isMoved = false;
	//寿命
	float m_lifeTime = 3.0f;
	//座標とか
	CVector3 m_pos, m_posOld, m_vector;
	//コリジョン
	DHCollision m_col;
};


//TODO 地形判定デコレーター
//TODO ムラ(MHW) 地形ノイズ
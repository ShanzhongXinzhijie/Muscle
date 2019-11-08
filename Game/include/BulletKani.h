#pragma once
#include"BeamModel.h"
#include"IFu.h"

class BulletGO;

/// <summary>
/// バレットに
/// ホーミング・特殊軌道
/// 接触処理・爆発
/// ↑この辺をデコるためのやつ
/// </summary>
class IBulletComponent {
public:
	//IBulletDecolator(IBulletDecolator* ptrDecolator = nullptr) :m_decolator(ptrDecolator) {}
	//virtual ~IBulletDecolator() { if (m_decolator) { delete m_decolator; m_decolator = nullptr; } }

	/*void DecoUpdate() {
		if (m_decolator)m_decolator->DecoUpdate();
		InnerUpdate();
	}
	void DecoPostUpdate() {
		if (m_decolator)m_decolator->DecoPostUpdate();
		InnerPostUpdate();
	}
	void DecoContact(SuicideObj::CCollisionObj::SCallbackParam& p) {
		if (m_decolator)m_decolator->DecoContact(p);
		InnerContact(p);
	}*/

	virtual void Update() {}
	virtual void PostUpdate()  {}
	virtual void Contact(SuicideObj::CCollisionObj::SCallbackParam& p) {}

	void SetBullet(BulletGO* bullet) {
		m_bullet = bullet;
		//if (m_decolator) {
		//	m_decolator->SetBullet(m_bullet);
		//}
	}
	/*BulletGO* GetBullet() {
		return m_bullet;		
	}*/

//private:
	//IBulletDecolator* m_decolator = nullptr;
protected:
	BulletGO* m_bullet = nullptr;
};

/// <summary>
/// バレットを動かすクラス
/// </summary>
class BulletGO : public IQSGameObject, public IFu {
public:
	BulletGO(const CVector3& pos, const CVector3& move);
	//~BulletGO() { if (m_decolator) { delete m_decolator; m_decolator = nullptr; } }

	void Update()override;
	void PostLoopUpdate()override;

	/// <summary>
	/// コンポーネントを追加する
	/// </summary>
	void AddComponent(std::unique_ptr<IBulletComponent> component) {
		m_components.emplace_back(std::move(component));
		m_components.back()->SetBullet(this);
	}

private:
	//デコレーター
	//IBulletDecolator* m_decolator = nullptr;
	//コンポーネント
	std::list<std::unique_ptr<IBulletComponent>> m_components;
	
	//モデル
	BeamModel m_model;
	bool m_isMoved = false;
	CVector3 m_lastDrawPos;	

	//座標とか
	CVector3 m_posOld;
	//CVector3 m_pos, m_posOld;

	//コリジョン
	DHCollision m_col;

public:
	//寿命
	float m_lifeTime = 3.0f;
	//進行ベクトル
	CVector3 m_vector;
};

/// <summary>
/// ホーミングデコレーター
/// </summary>
class BD_Homing : public IBulletComponent {
public:
	//TODO IFuの参照切れ
	BD_Homing(const IFu* target = nullptr) :m_target(target) {};

	void Update()override {
		if (!m_target) { return; }
		m_bullet->m_vector = (m_target->GetPos() - m_bullet->GetPos()).GetNorm()*m_bullet->m_vector.Length();
	}

private:
	const IFu* m_target = nullptr;
};

/// <summary>
/// 衝突で消滅デコレーター
/// </summary>
class BD_Contact : public IBulletComponent {
public:
	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//クラス取り出す
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical]) {
				//物理属性なら死
				m_bullet->m_lifeTime = 0.0f;
			}
		}
		if (!p.m_isCCollisionObj) {
			//相手がCCollisionObjじゃなくても死
			m_bullet->m_lifeTime = 0.0f;
		}
	}
};
/// <summary>
/// 衝突で爆発デコレーター
/// </summary>
class BD_ContactExplosion : public IBulletComponent {
public:	
	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//クラス取り出す
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical]) {
				//物理属性なら死
				m_bullet->m_lifeTime = 0.0f;
				//TODO 爆発生成
			}
		}
		if (!p.m_isCCollisionObj) {
			//相手がCCollisionObjじゃなくても死
			m_bullet->m_lifeTime = 0.0f;
			//TODO 爆発生成
		}
	}
};

//TODO 地形判定デコレーター
//TODO ムラ(MHW) 地形ノイズ
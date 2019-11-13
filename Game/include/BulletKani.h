#pragma once
#include"BeamModel.h"
#include"IFu.h"

class BulletGO;

/// <summary>
/// バレットコンポーネント
/// </summary>
class IBulletComponent {
public:
	virtual ~IBulletComponent() {}

	virtual void Start() {}
	virtual void Update() {}
	virtual void PostUpdate()  {}
	virtual void PostLoopUpdate() {}
	virtual void Contact(SuicideObj::CCollisionObj::SCallbackParam& p) {}

	//設定元を設定
	void SetBullet(BulletGO* bullet) {
		m_bullet = bullet;
	}
protected:
	BulletGO* m_bullet = nullptr;
};

/// <summary>
/// バレットを動かすクラス
/// </summary>
class BulletGO : public IGameObject, public IFu {
public:
	BulletGO(const CVector3& pos, const CVector3& move);

	void Update()override;
	void PostLoopUpdate()override;

	/// <summary>
	/// コンポーネントを追加する
	/// </summary>
	void AddComponent(std::unique_ptr<IBulletComponent> component) {
		m_components.emplace_back(std::move(component));
		m_components.back()->SetBullet(this);
		m_components.back()->Start();
	}

	/// <summary>
	/// 旧座標の取得
	/// </summary>
	const CVector3& GetOldPos()const { return m_posOld; }

private:
	//コンポーネント
	std::vector<std::unique_ptr<IBulletComponent>> m_components;	
	//古座標
	CVector3 m_posOld;
public:
	//寿命
	float m_lifeTime = 3.0f;
	//進行ベクトル
	CVector3 m_vector;
	//攻撃コリジョン
	DHCollision m_col;
};

/// <summary>
/// ビームモデルコンポーネント
/// </summary>
class BD_BeamModel : public IBulletComponent {
public:
	BD_BeamModel(float radius = 3.0f, std::wstring_view beamName = L"BLUE") :m_radius(radius){
		//モデル
		m_model.Init(beamName.data());
		m_model.SetRadius(radius);
	}
	void Start()override {
		//座標初期化
		m_model.SetPos(m_bullet->GetOldPos(), m_bullet->GetPos());
		m_lastDrawPos = m_bullet->GetPos();
		//攻撃判定作成
		m_bullet->m_col.m_collision.CreateSphere(m_bullet->GetPos(), {}, m_radius);
	}
	void PostLoopUpdate()override {
		//モデル更新
		if (!m_isMoved) {
			m_model.SetPos(m_bullet->GetOldPos(), m_bullet->GetPos()); m_isMoved = true;
		}
		else {
			m_model.Move(m_bullet->GetPos() - m_lastDrawPos);
		}
		m_lastDrawPos = m_bullet->GetPos();
	}
private:
	//モデル
	BeamModel m_model;
	float m_radius = 3.0f;
	bool m_isMoved = false;
	CVector3 m_lastDrawPos;
};

/// <summary>
/// ホーミングコンポーネント
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
/// 衝突で消滅コンポーネント
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
/// 衝突で爆発コンポーネント
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

//TODO ムラ(MHW) 地形ノイズ
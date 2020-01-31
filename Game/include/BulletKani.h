#pragma once
#include"IFu.h"

class BulletGO;

/// <summary>
/// バレットコンポーネント
/// </summary>
class IBulletComponent {
public:
	virtual ~IBulletComponent() {}

	virtual void Start() {}
	virtual void PreLoopUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate()  {}
	virtual void PostLoopUpdate() {}
	virtual void Pre3DRender(int) {}
	virtual void Contact(SuicideObj::CCollisionObj::SCallbackParam& p) {}
	virtual bool PreContact(ReferenceCollision* refcol) { return true; }

	//設定元を設定
	void SetBullet(BulletGO* bullet) {
		m_bullet = bullet;
	}

	//コンポーネントが有効か設定
	void SetEnable(bool b) {
		m_enable = b;
	}
	bool GetEnable()const {
		return m_enable;
	}

protected:
	bool m_enable = true;
	BulletGO* m_bullet = nullptr;
};

/// <summary>
/// バレットを動かすクラス
/// </summary>
class BulletGO : public ILifeObject, public ICreateObject {
public:
	BulletGO(const CVector3& pos, const CVector3& move, IFu* owner, float damege, bool isLockable = false, int priorityLevel = LockableWrapper::DEFAULT_LEVEL);

	void PreLoopUpdate()override;
	void Update()override;
	void PostLoopUpdate()override;
	void Pre3DRender(int)override;

	/// <summary>
	/// スタン時間を設定
	/// </summary>
	void SetStunTimeSec(float stunTimeSec) {
		m_col.m_reference.stunTimeSec = stunTimeSec;
	}

	/// <summary>
	/// コンポーネントを追加する
	/// </summary>
	void AddComponent(std::unique_ptr<IBulletComponent>&& component) {
		m_components.emplace_back(std::move(component));
		m_components.back()->SetBullet(this);
		m_components.back()->Start();
	}
	/// <summary>
	/// コンポーネントを取得
	/// </summary>
	IBulletComponent& GetComponent(int i) {
		return *m_components[i].get();
	}
	/// <summary>
	/// 最後に追加したコンポーネントを取得
	/// </summary>
	IBulletComponent& GetComponentBack() {
		return *m_components.back().get();
	}	

	/// <summary>
	/// オーナーの取得
	/// </summary>
	IFu* GetOwner() {
		return m_owner;
	}

	/// <summary>
	/// 旧座標の取得
	/// </summary>
	const CVector3& GetOldPos()const { return m_posOld; }

	/// <summary>
	/// 毎フレームのベロシティの変化を計算
	/// </summary>
	static void CalcVelocityUpdate(CVector3& velocity, float gravity = 0.0f, float upBrake = 0.025f, float downAccel = 0.25f);

private:
	//この弾のオーナー
	IFu* m_owner = nullptr;
	//コンポーネント
	std::vector<std::unique_ptr<IBulletComponent>> m_components;	
	//古座標
	CVector3 m_posOld;
	//初期座標
	CVector3 m_defaultPos;
	//初期攻撃力
	float m_defaultDamege = 0.0f;
	//ダメージブーストエフェクト
	std::unique_ptr<CBillboard> m_damegeBoostEffect;
public:
	//寿命
	float m_lifeTime = 3.0f*FRAME_RATE;
	//進行ベクトル
	CVector3 m_vector;
	//攻撃コリジョン
	DHCollision m_col;
	//半径
	float m_radius = 0.0f;
	//重力
	float m_gravity = 0.0f;
	//上昇減速
	float m_upBrake = 0.025f;
	//下降加速
	float m_downAccel = 0.25f;
	//高さによるダメージブーストの有効フラグ
	bool m_isDamegeHeightBoost = true;
};

/// <summary>
/// タイマーコンポーネント
/// 一定時間後にコンポーネントを有効化・無効化する
/// </summary>
/// <param name="timer">設定時間。実行までのフレーム数</param>
/// <param name="willEnable">trueで設定時間後コンポーネントを有効化。falseなら無効化</param>
class BD_Timer : public IBulletComponent {
public:
	BD_Timer(float timer = 0.0f, bool willEnable = true)
		: m_timerf(timer), m_willEnable(willEnable)
	{};

	void Update()override {
		if (m_timerf < FLT_EPSILON) { return; }
		//タイマー処理
		m_timerf = max(0.0f, m_timerf - 1.0f);
		if (m_timerf < FLT_EPSILON) {
			for (auto& com : m_components) {
				com->SetEnable(m_willEnable);
			}
		}
	}

	/// <summary>
	/// コンポーネントを追加
	/// </summary>
	void AddComponent(IBulletComponent* comp) {
		if (comp) {
			comp->SetEnable(!m_willEnable);//有効・無効の設定変更
			m_components.emplace_back(comp);
		}
	}

private:
	float m_timerf = 0.0f;
	bool m_willEnable = true;
	std::list<IBulletComponent*> m_components;
};

/// <summary>
/// ロックオン可能化コンポーネント
/// </summary>
class BD_Lockable :public IBulletComponent {
public:
	BD_Lockable(int lockLevel = LockableWrapper::DEFAULT_LEVEL) :m_lockLevel(lockLevel) {}

	void Start()override {
		m_bullet->SetLockable(true, m_bullet->GetOwner());
	}

private:
	int m_lockLevel = LockableWrapper::DEFAULT_LEVEL;
};
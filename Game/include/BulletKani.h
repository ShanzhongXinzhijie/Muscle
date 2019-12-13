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
	virtual void PreLoopUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate()  {}
	virtual void PostLoopUpdate() {}
	virtual void Pre3DRender(int) {}
	virtual void Contact(SuicideObj::CCollisionObj::SCallbackParam& p) {}

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
class BulletGO : public ILifeObject {
public:
	BulletGO(const CVector3& pos, const CVector3& move, bool isLockable = false);

	void PreLoopUpdate()override;
	void Update()override;
	void PostLoopUpdate()override;
	void Pre3DRender(int)override;

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

	/// <summary>
	/// 毎フレームのベロシティの変化を計算
	/// </summary>
	static void CalcVelocityUpdate(CVector3& velocity, float gravity);

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
	//半径
	float m_radius = 0.0f;
	//重力
	float m_gravity = 0.0f;
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
	void Start()override {
		m_bullet->SetLockable(true);
	}
};

/// <summary>
/// ビームモデルコンポーネント
/// ※画面分割数を変更した場合作り直す必要がある
/// </summary>
class BD_BeamModel : public IBulletComponent {
public:
	BD_BeamModel(float radius = 3.0f, std::wstring_view beamName = L"BLUE") :m_radius(radius){
		//モデル
		m_modelNum = GetScreenNum();
		m_model = std::make_unique<BeamModel[]>(m_modelNum);//画面の数だけ作成
		for (int i = 0; i < m_modelNum; i++) {
			m_model[i].Init(beamName.data());
			m_model[i].SetRadius(radius);
			m_model[i].SetIsDraw(false);
		}
	}
	void Start()override {
		//座標初期化
		m_lastDrawPos = m_bullet->GetPos();
		//攻撃判定作成
		m_bullet->m_col.m_collision.CreateSphere(m_bullet->GetPos(), {}, m_radius);
		m_bullet->m_radius = m_radius;
	}
	void PreLoopUpdate()override {
		//最後に描画した座標更新
		m_lastDrawPos = m_bullet->GetPos();
		//モデル表示初期化
		for (int i = 0; i < m_modelNum; i++) {
			m_model[i].SetIsDraw(true);
		}
	}
	void PostLoopUpdate()override{
		//モデルの更新(画面数分)
		for (int i = 0; i < m_modelNum; i++) {
			CVector3 rootPos, tipPos;
			GameObj::ICamera* screenCamera = m_modelNum == 1 ? GetMainCamera() : GetCameraList()[i];

			//カメラと相対的な座標を求める(お尻側)
			CVector3 soutaiPos = m_lastDrawPos;
			CMatrix viewMat = screenCamera->GetViewMatrixOld();
			viewMat.Mul(soutaiPos);
			viewMat = screenCamera->GetViewMatrix();
			viewMat.Inverse();
			viewMat.Mul(soutaiPos);
			rootPos = soutaiPos;

			//先端側の座標求める
			tipPos = m_lastDrawPos + (m_bullet->GetPos() - m_lastDrawPos);

			//blurScaleを適応
			constexpr float blurScale = 1.0f;
			rootPos = tipPos + (rootPos - tipPos)*blurScale;

			//モデル更新
			m_model[i].SetPos(rootPos, tipPos);
		}		
	}
	void Pre3DRender(int screenNum)override {
		//表示するモデルの切り替え
		for (int i = 0; i < m_modelNum; i++) {
			m_model[i].SetIsDraw(i == screenNum ? true : false);
		}		
	}
private:	
	int m_modelNum = 0;//モデルの数
	std::unique_ptr<BeamModel[]> m_model;//モデル
	float m_radius = 3.0f;//半径
	CVector3 m_lastDrawPos;//最後に描画した座標
};


/// <summary>
/// 脚モデルコンポーネント
/// </summary>
class BD_LegModel : public IBulletComponent {
public:
	//scaleにはホトケのスケールを入れればいいと思うよ
	BD_LegModel(const CQuaternion& rot, const CVector3& scale) : m_rot(rot), m_scale(scale*5.0f) {}

	void Start()override {
		//モデル作成
		m_model.Init(L"Resource/modelData/leg.cmo");
		m_model.SetPos(m_bullet->GetPos());
		m_model.SetRot(m_rot);
		m_model.SetScale(m_scale);
		//攻撃判定作成
		float radius = 50.0f*((m_scale.GetMin() + m_scale.GetMax())*0.5f);
		m_bullet->m_col.m_collision.CreateSphere(m_bullet->GetPos(), {}, radius);
		m_bullet->m_radius = radius;
	}
	void PostLoopUpdate()override {
		//モデルの更新
		m_model.SetPos(m_bullet->GetPos());
	}

private:
	CQuaternion m_rot;
	CVector3 m_scale = 1.0f;
	GameObj::CSkinModelRender m_model;//モデル
};

/// <summary>
/// ホーミングコンポーネント
/// </summary>
class BD_Homing : public IBulletComponent {
public:
	BD_Homing(IFu* target = nullptr, float thrust = 0.0f, float noAccelRad = 0.0f, float timer = 0.0f)
		: m_target(target), m_thrust(thrust), m_nonAccelRad(noAccelRad), m_timerf(timer)
	{
		if (target) { target->SetDeleteFlag(isTargetDeath); }
	};

	void Update()override {
		//タイマー処理
		m_timerf = max(0.0f, m_timerf - 1.0f);
		if (m_timerf > 0.0f) { return; }

		CVector3 targetDir(m_bullet->m_vector);
		
		//目標あり ＆ 目標のインスタンスが消滅していない
		if (m_target && !isTargetDeath) {
			targetDir = m_target->GetPos() - m_bullet->GetPos();
		}

		targetDir.Normalize();

		//常に加速設定 or 目標方向との角度が開いている
		if (m_nonAccelRad < FLT_EPSILON || CVector3::AngleOf2NormalizeVector(targetDir,m_bullet->m_vector.GetNorm()) > m_nonAccelRad) {
			CVector3 beforeVec = m_bullet->m_vector;
			m_bullet->m_vector += targetDir * m_thrust;
			if (m_bullet->m_vector.LengthSq() < FLT_EPSILON) {//停止はしない
				m_bullet->m_vector = beforeVec;
			}
		}
	}

	//推力を設定
	void SetThrust(float f) {
		m_thrust = f;
	}

private:
	const IFu* m_target = nullptr;//目標
	std::shared_ptr<bool> isTargetDeath = false;//ターゲットのインスタンスが存在するか?
	float m_thrust = 0.0f;//推力
	float m_nonAccelRad = 0.0f;//加速しない角度範囲
	float m_timerf = 0.0f;//ホーミング開始までのフレーム数
};


/// <summary>
/// 減速コンポーネント
/// </summary>
class BD_Brake : public IBulletComponent {
public:
	BD_Brake(float brakePow):m_brakePawer(max(0.0f,brakePow)){}

	void Update()override {
		m_bullet->m_vector = m_bullet->m_vector.GetNorm()*max(0.0f, m_bullet->m_vector.Length() - m_brakePawer);
	}

private:
	float m_brakePawer = 0.0f;//1フレームに減速する速度
};

/// <summary>
/// 衝突で消滅コンポーネント
/// </summary>
class BD_Contact : public IBulletComponent {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="isContactField">地面と衝突するか?</param>
	BD_Contact(bool isContactField = true) : m_isContactField (isContactField){}

	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//クラス取り出す
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical] && (m_isContactField || !H->attributes[enGraund])) {
				//物理属性なら死
				m_bullet->m_lifeTime = 0.0f;
				m_bullet->SetPos(p.m_collisionPoint);
			}
		}
		if (!p.m_isCCollisionObj) {
			//相手がCCollisionObjじゃなくても死
			m_bullet->m_lifeTime = 0.0f;
			m_bullet->SetPos(p.m_collisionPoint);
		}
	}
private:
	bool m_isContactField = true;
};
/// <summary>
/// 跳弾コンポーネント
/// </summary>
class BD_Reflect : public IBulletComponent {
public:
	void PostUpdate()override {
		//レイで判定
		btVector3 rayStart = m_bullet->GetOldPos();
		btVector3 rayEnd = m_bullet->GetPos();
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			CVector3 Nvec = m_bullet->m_vector.GetNorm();
			//入射ベクトル多少ランダムに
			CVector3 randamAxis = { CMath::RandomZeroToOne() - 0.5f,CMath::RandomZeroToOne() - 0.5f ,CMath::RandomZeroToOne() - 0.5f };
			randamAxis.Normalize();
			CQuaternion randamRot(randamAxis, CMath::RandomZeroToOne()*0.5f);
			randamRot.Multiply(Nvec);
			//反射ベクトル
			m_bullet->m_vector = CMath::CalcReflectVector(Nvec, gnd_ray.m_hitNormalWorld) * m_bullet->m_vector.Length();
			m_bullet->SetPos(gnd_ray.m_hitPointWorld + gnd_ray.m_hitNormalWorld * (m_bullet->m_radius+1.0f));
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
				m_bullet->SetPos(p.m_collisionPoint);
				//TODO 爆発生成
			}
		}
		if (!p.m_isCCollisionObj) {
			//相手がCCollisionObjじゃなくても死
			m_bullet->m_lifeTime = 0.0f;
			m_bullet->SetPos(p.m_collisionPoint);
			//TODO 爆発生成
		}
	}
};
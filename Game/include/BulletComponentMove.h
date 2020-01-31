#pragma once
#include"BulletKani.h"

/// <summary>
/// 追尾コンポーネント
/// </summary>
class BD_Tracking : public IBulletComponent {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="target">追尾対象</param>
	BD_Tracking(LockableWrapper* target = nullptr)
		: m_target(target)
	{
		if (target) {
			//ターゲットの消滅通知を受け取れるようにする
			isTargetDeath = std::make_shared<bool>(false);
			target->GetFu()->SetDeleteFlag(isTargetDeath);
		}
	};

	void Update()override {
		//進行方向を算出
		CVector3 targetDir(m_bullet->m_vector);
		//目標あり ＆ 目標のインスタンスが消滅していない
		if (m_target && !(*isTargetDeath.get())) {
			targetDir = m_target->GetFu()->GetCollisionPos() - m_bullet->GetPos() + m_target->GetMove();
		}
		targetDir.Normalize();

		//角度が一定以下で追跡
		if (CVector3::AngleOf2NormalizeVector(targetDir, m_bullet->m_vector.GetNorm()) < CMath::DegToRad(25.0f)) {
			//targetDir.Lerp(0.5f, m_bullet->m_vector.GetNorm(), targetDir);
			//targetDir.Normalize();
			m_bullet->m_vector = targetDir * m_bullet->m_vector.Length();
		}
	}

private:
	const LockableWrapper* m_target = nullptr;//目標
	std::shared_ptr<bool> isTargetDeath;//ターゲットのインスタンスが存在するか?
};

/// <summary>
/// ホーミングコンポーネント
/// </summary>
class BD_Homing : public IBulletComponent {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="target">追跡目標</param>
	/// <param name="thrust">1fの推進力</param>
	/// <param name="noAccelRad">目標との角度がこれ以下だと加速しない</param>
	/// <param name="limitRad">目標を追跡可能な角度</param>
	/// <param name="timer">追跡開始までの時間</param>
	BD_Homing(LockableWrapper* target = nullptr, float thrust = 0.0f, float noAccelRad = 0.0f, float limitRad = CMath::PI2, float timer = 0.0f)
		: m_target(target), m_thrust(thrust), m_nonAccelRad(noAccelRad), m_limitRad(limitRad), m_timerf(timer)
	{
		if (target) {
			//ターゲットの消滅通知を受け取れるようにする
			isTargetDeath = std::make_shared<bool>(false);
			target->GetFu()->SetDeleteFlag(isTargetDeath);
		}

		m_arrowModel.SetEnable(false);
	};

	/// <summary>
	/// 初期化方向を設定
	/// </summary>
	void SetInitDirection(const CVector3& dir) {
		m_isInitedDir = true;
		m_initDir = dir.GetNorm();
	}

	void Start()override {
		//追尾方向の初期化
		if (!m_isInitedDir) {
			SetInitDirection(m_bullet->m_vector);
		}
		if (m_timerf > 0.0f) {
			//初期化方向表示モデル
			m_arrowModel.SetEnable(true);
			m_arrowModel.Init(L"Resource/modelData/arrow.cmo");
			//m_arrowModel.InitPostDraw(PostDrawModelRender::enBlendMode::enAlpha);
			m_arrowModel.GetSkinModel().FindMaterialSetting(
				[&](MaterialSetting* mat) {
					mat->SetIsMotionBlur(false);
					mat->SetLightingEnable(false);
				}
			);
			m_arrowModel.SetPos(m_bullet->GetPos());
			CQuaternion rot;
			rot.MakeLookToUseXYAxis(m_initDir*-1.0f);
			m_arrowModel.SetRot(rot);
			m_arrowModel.SetScale(2.0f);
		}
	}

	void Update()override {
		//最後に移動した方向を記録
		if (m_bullet->m_vector.LengthSq() > FLT_EPSILON) {
			m_lastDir = m_bullet->m_vector.GetNorm();
		}

		//タイマー処理
		if (m_timerf > 0.0f) {
			m_timerf = max(0.0f, m_timerf - 1.0f);
			if (m_timerf > 0.0f) {
				//初期化方向表示
				//座標
				m_arrowModel.SetPos(m_bullet->GetPos());
				//点滅
				m_isDrawTimer--;
				if (m_isDrawTimer <= 0) {
					m_isDrawTimer = !m_arrowModel.GetIsDraw() ? 10 : 4;
					m_arrowModel.SetIsDraw(!m_arrowModel.GetIsDraw());
				}
				return;
			}
			else {
				//初期化方向表示やめ
				m_arrowModel.SetEnable(false);
				//方向初期化
				m_bullet->m_vector = m_initDir * max(1.0f, m_bullet->m_vector.Length());
			}
		}

		//目標方向
		CVector3 targetDir(m_bullet->m_vector);
		//目標あり ＆ 目標のインスタンスが消滅していない
		if (m_target && !(*isTargetDeath.get())) {
			targetDir = m_target->GetFu()->GetCollisionPos() - m_bullet->GetPos() + m_target->GetMove();
		}
		targetDir.Normalize();

		//常に加速設定 or 目標方向との角度が開いている and 誘導角度内
		float rad = 0.0f;
		float dirLength = m_bullet->m_vector.LengthSq();
		if (dirLength > FLT_EPSILON) {
			rad = CVector3::AngleOf2NormalizeVector(targetDir, m_bullet->m_vector.GetNorm());
		}
		else {
			targetDir = m_lastDir;
		}
		if (dirLength < FLT_EPSILON || ((m_nonAccelRad < FLT_EPSILON || rad > m_nonAccelRad) && rad < m_limitRad)) {
			//ホーミング処理
			CVector3 beforeVec = m_bullet->m_vector;

			if (dirLength > FLT_EPSILON) {
				//ブレーキング
				float brakePow = CMath::Saturate(targetDir.Dot(m_bullet->m_vector.GetNorm()));
				m_bullet->m_vector *= brakePow;
				//m_bullet->m_vector -= m_bullet->m_vector.GetNorm()* m_thrust * 2.0f *(1.0f - brakePow);
			}

			//目標へ加速
			m_bullet->m_vector += targetDir * m_thrust;
			//停止はしない
			if (m_bullet->m_vector.LengthSq() < FLT_EPSILON) {
				m_bullet->m_vector = beforeVec;
			}
		}
		else if (m_nonAccelRad < FLT_EPSILON) {
			//ただの加速
			m_bullet->m_vector += m_bullet->m_vector.GetNorm() * m_thrust;
		}
	}

	//推力を設定
	void SetThrust(float f) {
		m_thrust = f;
	}

private:
	const LockableWrapper* m_target = nullptr;//目標
	std::shared_ptr<bool> isTargetDeath;//ターゲットのインスタンスが存在するか?
	float m_thrust = 0.0f;//推力
	float m_nonAccelRad = 0.0f;//加速しない角度範囲
	float m_limitRad = CMath::PI2;//追尾可能な角度
	float m_timerf = 0.0f;//ホーミング開始までのフレーム数
	CVector3 m_initDir;//初期化方向
	bool m_isInitedDir = false;//初期化方向が設定されているか
	CVector3 m_lastDir = CVector3::Down();//最後に移動した方向

	//初期化方向表示
	GameObj::CSkinModelRender m_arrowModel;
	int m_isDrawTimer = 0;
};


/// <summary>
/// 減速コンポーネント
/// </summary>
class BD_Brake : public IBulletComponent {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="brakePow">1fに減速する速度</param>
	BD_Brake(float brakePow) :m_brakePawer(max(0.0f, brakePow)) {}

	void Update()override {
		//減速
		m_bullet->m_vector = m_bullet->m_vector.GetNorm()*max(0.0f, m_bullet->m_vector.Length() - m_brakePawer);
	}

private:
	float m_brakePawer = 0.0f;//1フレームに減速する速度
};

/// <summary>
/// 地形衝突回避コンポーネント
/// </summary>
class BD_ClashAvoidance : public IBulletComponent {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="thrust">推力</param>
	/// <param name="avoidanceFrame">何フレーム先の衝突まで回避するか</param>
	BD_ClashAvoidance(float thrust,float avoidanceFrame = 3.0f): m_thrust(thrust), m_avoidanceFrame(avoidanceFrame){}

	void PostUpdate()override {
		//レイで判定
		btVector3 rayStart = m_bullet->GetPos();
		btVector3 rayEnd = m_bullet->GetPos() + m_bullet->m_vector * m_avoidanceFrame;

		if ((rayStart - rayEnd).fuzzyZero()) {
			return;
		}

		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		//このままでは衝突する!
		if (gnd_ray.hasHit()) {

			CVector3 Nvec = m_bullet->m_vector.GetNorm();
			//入射ベクトル多少ランダムに
			/*CVector3 randamAxis = { CMath::RandomZeroToOne() - 0.5f,CMath::RandomZeroToOne() - 0.5f ,CMath::RandomZeroToOne() - 0.5f };
			randamAxis.Normalize();
			CQuaternion randamRot(randamAxis, CMath::RandomZeroToOne()*0.5f);
			randamRot.Multiply(Nvec);*/
			//反射ベクトル
			m_bullet->m_vector = CMath::CalcReflectVector(Nvec, gnd_ray.m_hitNormalWorld).GetNorm() * m_bullet->m_vector.Length();
			
			//軌道変える
			//CVector3 targetDir = CVector3::Up();
			//m_bullet->m_vector = targetDir * m_bullet->m_vector.Length();
			/*
			//ブレーキング
			float brakePow = CMath::Saturate(targetDir.Dot(m_bullet->m_vector.GetNorm()));
			m_bullet->m_vector *= brakePow;
			//目標へ加速
			m_bullet->m_vector += targetDir * m_thrust;
			*/
		}
	}

private:
	float m_thrust = 0.0f;//推力
	float m_avoidanceFrame = 10.0f;//何フレーム先の衝突まで回避するか
};
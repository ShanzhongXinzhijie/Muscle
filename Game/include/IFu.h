#pragma once

/// <summary>
/// ゲームフィールド上に存在するものを表すインターフェースクラス
/// </summary>
class IFu 
{
public:
	//コンストラクタ
	IFu();
	//デストラクタ
	virtual ~IFu() {
		//死亡フラグを立てて削除を通知する
		for (auto& flag : m_deleteFlagList) {
			if (!flag.expired()) {//削除されてなければ
				*flag.lock() = true;
			}
		}
	}

	//通知用死亡フラグを設定
	void SetDeleteFlag(std::weak_ptr<bool> flagPtr) {
		m_deleteFlagList.push_back(flagPtr);
	}

	//位置を設定
	void SetPos(const CVector3& pos) { m_pos = pos; UpdateCollisionPos(); }
	//回転を設定
	void SetRot(const CQuaternion& rot) {
		m_rot = rot; m_col.SetRot(m_colRot * m_rot);
		m_front = CVector3::Front(), m_up = CVector3::Up(), m_left = CVector3::Left();
		m_rot.Multiply(m_front);
		m_rot.Multiply(m_up);
		m_rot.Multiply(m_left);
		m_back = m_front * -1.0f, m_down = m_up * -1.0f, m_right = m_left * -1.0f;
	}

	//移動
	void Move(const CVector3& move) { m_pos += move; UpdateCollisionPos(); }

	//位置を取得
	const CVector3& GetPos()const { return m_pos; }
	//コリジョン位置を取得
	CVector3 GetCollisionPos()const { return m_pos + m_colPos; }
	//回転を取得
	const CQuaternion& GetRot()const { return m_rot; }

	//前方向を取得
	const CVector3& GetFront()const { return m_front; }
	//後方向を取得
	const CVector3& GetBack()const { return m_back; }
	//上方向を取得
	const CVector3& GetUp()const { return m_up; }
	//下方向を取得
	const CVector3& GetDown()const { return m_down; }
	//左方向を取得
	const CVector3& GetLeft()const { return m_left; }
	//右方向を取得
	const CVector3& GetRight()const { return m_right; }

//protected:	
	/// <summary>
	/// DHCollisionとの衝突時に実行する関数
	/// </summary>
	void SetCollisionFunc(std::function<void(ReferenceCollision*, SuicideObj::CCollisionObj::SCallbackParam&)> func) {
		m_collisionFunc = func;
	}
	/// <summary>
	/// CollisionFunc(↑)の前に実行する関数
	/// </summary>
	void SetPreCollisionFunc(std::function<void(ReferenceCollision*)> func) {
		m_col.m_reference.m_preCollisionFunc = func;
	}

	//この喰らい判定を静的オブジェクトとしても初期化するか設定
	//※当たり判定形状を作成する前に設定してください
	void SetIsStaticObject(bool isStatic) {
		m_col.m_collision.SetIsStaticObject(isStatic);
	}

	//球形の喰らい判定を作成
	void CreateSphere(const CVector3& pos, const CQuaternion& rot, float radius) {
		m_colPos = pos; m_colRot = rot;
		m_col.m_collision.CreateSphere(m_pos + m_colPos, m_colRot * m_rot, radius);
	}
	//カプセル形の喰らい判定を作成
	void CreateCapsule(const CVector3& pos, const CQuaternion& rot, float radius, float height) {
		m_colPos = pos; m_colRot = rot;
		m_col.m_collision.CreateCapsule(m_pos + m_colPos, m_colRot * m_rot, radius, height);
	}
	//メッシュ形の喰らい判定を作成
	void CreateMesh(const GameObj::CSkinModelRender& skinModelRender) {
		m_colPos = skinModelRender.GetPos(); m_colRot = skinModelRender.GetRot();
		m_col.m_collision.CreateMesh(skinModelRender);
	}

	//喰らい判定の属性ビットマスクを取得
	std::bitset<enAttributesNum>& GetAttributes() {
		return m_col.m_reference.attributes;
	}
	//喰らい判定の攻撃力を設定
	void SetDamegePower(float dame) {
		m_col.m_reference.damege = dame;
	}
	//喰らい判定のスタン秒数を設定
	void SetStunSec(float sec) {
		m_col.m_reference.stunTimeSec = sec;
	}
	//喰らい判定のベロシティパラメータを設定
	void SetCollisionVelocity(const CVector3& velocity) {
		m_col.SetVelocity(velocity);
	}
	//喰らい判定のパラメータを取得
	const ReferenceCollision& GetReferenceCollision()const {
		return m_col.m_reference;
	}

	//喰らい判定の判定グループを設定
	void On_OneGroup(unsigned int oneGroup) {
		m_col.On_OneGroup(oneGroup);
	}

	//喰らい判定の位置オフセットを設定
	void SetCollisionPosOffset(const CVector3& pos) {
		m_colPos = pos;
		UpdateCollisionPos();
	}

	//喰らい判定の有効無効を設定
	void SetCollisionEnable(bool isEnable) {
		m_col.SetEnable(isEnable);
	}

private:
	//喰らい判定の位置を更新
	void UpdateCollisionPos() {
		CVector3 offset = m_colPos;
		m_rot.Multiply(offset);
		m_col.SetPos(m_pos + offset);
	}

private:
	//位置と回転
	CVector3 m_pos;
	CQuaternion m_rot;

	//各方向
	CVector3 m_front = CVector3::Front(), m_up = CVector3::Up(), m_left = CVector3::Left();
	CVector3 m_back = CVector3::Back(), m_down = CVector3::Down(), m_right = CVector3::Right();
	
	//コリジョン
	DHCollision m_col;
	CVector3 m_colPos;
	CQuaternion m_colRot;
	std::function<void(ReferenceCollision*, SuicideObj::CCollisionObj::SCallbackParam&)> m_collisionFunc;

	//通知
	std::list<std::weak_ptr<bool>> m_deleteFlagList;
};

//ロックできるオブジェクトラッパー
class LockableWrapper : public IQSGameObject {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="fu">ロック対象</param>
	/// <param name="hpPtr">HPのポインタ</param>
	/// <param name="owner">ロック対象のオーナー</param>
	LockableWrapper(IFu* fu, float* hpPtr = nullptr, IFu* owner = nullptr)
		: m_fu(fu),m_hpPtr(hpPtr),m_owner(owner)
	{
		SetName(L"LockableObject");
	}

	//HPを設定
	void SetHPRef(float* hpPtr) {
		m_hpPtr = hpPtr;
	}

	//毎フレームの処理
	void PreUpdate()override {
		if (m_isFirstUpdate) {
			m_isFirstUpdate = false;
			m_oldPos = m_fu->GetPos();
			return;
		}
		m_move = m_fu->GetPos() - m_oldPos;
		m_oldPos = m_fu->GetPos();
	}

	//本体を取得
	const IFu* GetFu()const {
		return m_fu;
	}
	IFu* GetFu() {
		return m_fu;
	}

	//移動量を取得
	const CVector3& GetMove()const {
		return m_move;
	}

	//HPを取得
	float GetHP()const {
		if (m_hpPtr) {
			return *m_hpPtr;
		}
		return -1.0f;
	}

	//ロック対象のオーナーを取得
	const IFu* GetOwner()const {
		return m_owner;
	}

private:
	IFu* m_fu = nullptr;
	IFu* m_owner = nullptr;

	bool m_isFirstUpdate = true;
	CVector3 m_move;//移動量(偏差射撃に使う)
	CVector3 m_oldPos;

	float* m_hpPtr = nullptr;
};

/// <summary>
/// 生命体オブジェクト
/// </summary>
class ILifeObject : public IGameObject, public IFu {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="isLockable">ロックオン可能か?</param>
	/// <param name="owner">このオブジェクトのオーナー</param>
	ILifeObject(bool isLockable = true, IFu* owner = nullptr) {
		if (isLockable) {
			m_lockableWrapper = std::make_unique<LockableWrapper>(this, nullptr, owner);
		}
	}

	//ロックオン用クラス取得
	LockableWrapper* GetLockableWrapper() {
		return m_lockableWrapper ? m_lockableWrapper.get() : nullptr;
	}

	//HPを設定
	void SetHPRef(float* hpPtr) {
		if (m_lockableWrapper) {
			m_lockableWrapper->SetHPRef(hpPtr);
		}
	}

	//ロックオン可能か設定
	void SetLockable(bool isLockable, IFu* owner) {
		if (isLockable) {
			if (!m_lockableWrapper) {
				m_lockableWrapper = std::make_unique<LockableWrapper>(this, nullptr, owner);
			}
		}
		else {
			m_lockableWrapper.reset();
		}
	}

	//死ぬ
	void Death() {
		if (m_deathFunc) { m_deathFunc(); }
		SetEnable(false);//ゲームオブジェクト無効化
		SetCollisionEnable(false);//当たり判定無効
		if (m_isAutoDelete) { delete this; return; }//自殺
	}

	//死亡時にインスタンスを削除するか設定
	void SetIsAutoDelete(bool isDelete) {
		m_isAutoDelete = isDelete;
	}

	//死亡時に実行する関数を設定
	void SetDeathFunc(std::function<void()> func) {
		m_deathFunc = func;
	}

private:
	bool m_isAutoDelete = true;
	std::function<void()> m_deathFunc;
	std::unique_ptr<LockableWrapper> m_lockableWrapper;
};
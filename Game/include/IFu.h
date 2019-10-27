#pragma once

/// <summary>
/// ゲームフィールド上に存在するものを表すインターフェースクラス
/// </summary>
class IFu 
{
public:
	//コンストラクタ
	IFu();

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

	//喰らい判定の判定グループを設定
	void On_OneGroup(unsigned int oneGroup) {
		m_col.On_OneGroup(oneGroup);
	}

	//喰らい判定の位置オフセットを設定
	void SetCollisionPos(const CVector3& pos) {
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
};

/// <summary>
/// 生命体オブジェクト
/// </summary>
class ILifeObject : public IGameObject, public IFu {
public:
	ILifeObject() {
		SetName(L"LockableObject");
	}

	//死ぬ
	void Death() {
		if (m_deathFunc) { m_deathFunc(); }
		SetEnable(false);//ゲームオブジェクト無効化
		SetCollisionEnable(false);//当たり判定無効
		if (m_isAutoDelete) { delete this; }//自殺
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
};
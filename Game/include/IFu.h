#pragma once

/// <summary>
/// ゲームフィールド上に存在するものを表すインターフェースクラス
/// </summary>
class IFu 
{
public:
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

protected:	
	//位置を取得
	//CVector3& GetPosRef() { return m_pos; }
	//回転を取得
	//CQuaternion& GetRotRef() { return m_rot; }

	/// <summary>
	/// DHCollisionとの衝突時に実行する関数
	/// </summary>
	void SetCollisionFunc(std::function<void(ReferenceCollision*)> func) {
		m_collisionFunc = func;
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
	//喰らい判定の位置オフセットを設定
	void SetCollisionPos(const CVector3& pos) {
		m_colPos = pos;
		UpdateCollisionPos();
	}

private:
	//喰らい判定の位置を更新
	void UpdateCollisionPos() {
		CVector3 offset = m_colPos;
		m_rot.Multiply(offset);
		m_col.SetPos(m_pos + offset);
	}

private:
	CVector3 m_pos;
	CQuaternion m_rot;
	//DW_GETSETCONSREF(CVector3, m_scale, Scale)

	CVector3 m_front = CVector3::Front(), m_up = CVector3::Up(), m_left = CVector3::Left();
	CVector3 m_back = CVector3::Back(), m_down = CVector3::Down(), m_right = CVector3::Right();
	
	DHCollision m_col;
	CVector3 m_colPos;
	CQuaternion m_colRot;
	std::function<void(ReferenceCollision*)> m_collisionFunc;
};


#pragma once

/// <summary>
/// ゲームフィールド上に存在するものを表すインターフェースクラス
/// </summary>
class IFu 
{
public:
	IFu();

protected:
	//位置を設定
	void SetPos(const CVector3& pos) { m_pos = pos; m_col.SetPos(pos); }
	//回転を設定
	void SetRot(const CQuaternion& rot) { m_rot = rot; m_col.SetRot(rot); }

	/// <summary>
	/// DHCollisionとの衝突時に実行する関数
	/// </summary>
	void SetCollisionFunc(std::function<void(ReferenceCollision*)> func) {
		m_collisionFunc = func;
	}

private:
	CVector3 m_pos;
	CQuaternion m_rot;
	//DW_GETSETCONSREF(CVector3, m_scale, Scale)
	
	DHCollision m_col;
	std::function<void(ReferenceCollision*)> m_collisionFunc;
};


#pragma once

#include<bitset>

//コリジョンの属性
enum enCollisionAttributes {
	enPhysical,
	enFlame,
	enWind,
	enWater,

	enAttributesNum,
};

/// <summary>
/// 当たり判定で参照する構造体
/// </summary>
struct ReferenceCollision : public IDW_Class {
public:	
	std::bitset<enAttributesNum> attributes;//属性ビットマスク
	CVector3 position;//位置
	CVector3 direction;//方向

	float damege = 0.0f;//ダメージ
};

/// <summary>
/// このゲームの当たり判定オブジェクト
/// </summary>
class DHCollision
{
public:
	//コンストラクタ
	DHCollision() {
		//衝突時に参照する構造体を設定
		m_collision.SetName(L"ReferenceCollision");
		m_collision.SetClass(&m_reference);
	}

	//座標を設定
	void SetPos(const CVector3& pos) {
		m_collision.SetPosition(pos);
		m_reference.position = pos;
	}
	//回転を設定
	void SetRot(const CQuaternion& rot) {
		m_collision.SetRotation(rot);
	}

	//方向を設定
	void SetDir(const CVector3& dir) {
		m_reference.direction = dir;
	}

	//有効無効の設定
	void SetEnable(bool isEnable) {
		m_collision.SetEnable(isEnable);
	}
	
	//コリジョンオブジェクト
	SuicideObj::CCollisionObj m_collision;
	//コリジョン情報
	ReferenceCollision m_reference;
};


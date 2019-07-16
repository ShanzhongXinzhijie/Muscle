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
};

/// <summary>
/// このゲームの当たり判定オブジェクト
/// </summary>
class DHCollision
{
public:
	DHCollision() {
		//衝突時に参照する構造体を設定
		m_collision.SetName(L"ReferenceCollision");
		m_collision.SetClass(&m_reference);
	}

	void SetPos(const CVector3& pos) {
		m_collision.SetPosition(pos);
		m_reference.position = pos;
	}

	SuicideObj::CCollisionObj m_collision;
	ReferenceCollision m_reference;
};


#pragma once

/// <summary>
/// 当たり判定で参照する構造体
/// </summary>
class ReferenceCollision : public IDW_Class {
public:
	enum enAttributes {
		enPhysical,
		enFlame,
		enWind,
		enWater,

		enAttributesNum,
	};
	unsigned int attributes = 0;//属性ビットマスク
	CVector3 position;
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

	SuicideObj::CCollisionObj m_collision;
	ReferenceCollision m_reference;
};


#pragma once

#include<bitset>

//コリジョンのグループ
enum enCollisionGroups {
	enDefault,
	enField,//地形

	enGroupsNum,
};

//コリジョンの属性
enum enCollisionAttributes {
	enPhysical,
	enGraund,
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
	CVector3 velocity;//ベロシティ
	CVector3 direction;//方向

	float damege = 0.0f;//ダメージ

	std::function<void(ReferenceCollision*)> m_preCollisionFunc;
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
	void SetVelocity(const CVector3& vec) {
		m_reference.velocity = vec;
		m_reference.direction = vec; m_reference.direction.Normalize();
	}

	//判定グループを設定
	void On_OneGroup(unsigned int oneGroup) {
		m_collision.On_OneGroup(oneGroup);
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

namespace DHUtil {
	/// <summary>
	/// 体当たりダメージを計算
	/// </summary>
	/// <param name="mineVelocity">ダメージを与える側のベロシティ</param>
	/// <param name="enemyVelocity">くらう側のベロシティ</param>
	static inline float CalcRamDamege(const CVector3& mineVelocity, const CVector3& enemyVelocity) {
		return max(0.0f, mineVelocity.GetNorm().Dot(mineVelocity - enemyVelocity));
	}
}
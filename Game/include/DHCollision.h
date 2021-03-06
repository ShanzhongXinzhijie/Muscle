#pragma once

#include<bitset>

//コリジョンのグループ
enum enCollisionGroups {
	enDefault,

	enField,//地形

	//ホトケたちよ
	/*enHotoke0,
	enHotoke1,
	enHotoke2,
	enHotoke3,*/

	enGroupsNum,
};
//static inline constexpr int enCollisionGroupsHotokeNum = 4;

//コリジョンの属性
enum enCollisionAttributes {
	enPhysical,
	enGraund,
	enFlame,
	enWind,
	enWater,

	enHotokeBody,

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
	float stunTimeSec = 0.0f;//スタン秒数

	unsigned long long ownerID = 0;
	unsigned long long nonHitID = 1;
	//const ReferenceCollision* nonHitCollision = nullptr;//この対象とは当たらない…

	CVector3 offsetPos;//なんかオフセット

	std::function<bool(ReferenceCollision*)> m_preCollisionFunc;//衝突前に実行する処理
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
		m_collision.SetCallback(
			[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
				if (!m_callbackFunction) { return; }
				//衝突できる相手か判定
				unsigned long long ownerID = 0, nonHitID = 1;
				if (p.EqualName(L"ReferenceCollision")){
					ReferenceCollision* ref = p.GetClass<ReferenceCollision>();
					ownerID = ref->ownerID;
					nonHitID = ref->nonHitID;
					if (m_reference.nonHitID == ownerID || m_reference.ownerID == nonHitID) {
						return;
					}
				}
				m_callbackFunction(p);
			}
		);
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
	//どのグループにも属さないよう設定
	void All_Off_Group() {
		m_collision.All_Off_Group();
	}
	//判定マスクを設定
	void Off_OneMask(unsigned int oneMask) {
		m_collision.Off_OneMask(oneMask);
	}

	//有効無効の設定
	void SetEnable(bool isEnable) {
		m_collision.SetEnable(isEnable);
	}
	
	//コリジョンオブジェクト
	SuicideObj::CCollisionObj m_collision;
	//コリジョン情報
	ReferenceCollision m_reference;

	//衝突時に実行する処理を設定
	void SetCollisionCallback(std::function<void(SuicideObj::CCollisionObj::SCallbackParam&)> callbackFunction) {
		m_callbackFunction = callbackFunction;
	}

private:
	//衝突時に実行する処理
	std::function<void(SuicideObj::CCollisionObj::SCallbackParam&)> m_callbackFunction;
};

namespace DHUtil {
	struct RamReturn {
		RamReturn() = default;
		RamReturn(float p_damege, float p_stunSec) : damege(p_damege), stunSec(p_stunSec) {};
		float damege = 0.0f;
		float stunSec = 0.0f;
	};
	/// <summary>
	/// 体当たりダメージを計算
	/// </summary>
	/// <param name="mineVelocity">ダメージを与える側のベロシティ</param>
	/// <param name="enemyVelocity">くらう側のベロシティ</param>
	/// <returns>ダメージとスタン値</returns>
	static inline RamReturn CalcRamDamege(const CVector3& mineVelocity, const CVector3& enemyVelocity) {
		float damege = max(0.0f, mineVelocity.GetNorm().Dot(mineVelocity - enemyVelocity));
		return damege > 10.0f ? RamReturn(damege, 1.0f) : RamReturn(0.0f, 0.0f);
	}
}
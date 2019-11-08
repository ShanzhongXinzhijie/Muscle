#pragma once
#include "BodyPartsHeader.h"

/// <summary>
/// カニ腕
/// </summary>
class BP_KaniArm :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	
	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;

	//アクション
	void ChargeAndMachinegun(enLR);
	void Rocket(enLR);
	void Lazer(enLR);
	void Stab();

	//チャージを取得
	int GetChargeTimer(enLR lr)const { return m_chargeTime[lr]; }

private:
	//コントローラー
	IBodyController<BP_KaniArm>* m_controller = nullptr;

	//ボーン関係
	AnimationClip m_initPose;
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};
	CVector3 m_ikTargetPos[enLRNUM] = {};
	int m_muzzleBoneID[enLRNUM] = {};

	//エフェクト
	CBillboard m_muzzleFlash[enLRNUM]; 
	int m_muzzleTime[enLRNUM] = {};

	//チャージタイマー
	bool m_isCharging[enLRNUM] = {}, m_isMachineGunning[enLRNUM] = {};
	int m_chargeTime[enLRNUM] = {};
	static constexpr int MACHINE_GUN_CHARGE_TIME = 10, LAZER_CHARGE_TIME = MACHINE_GUN_CHARGE_TIME - 3;
};

/// <summary>
/// カニ腕 #人コントローラー
/// </summary>
class HCon_KaniArm : public IBodyController<BP_KaniArm> {
public:
	using IBodyController::IBodyController;	
	void Update()override;
};

/// <summary>
/// カニ腕 #AIコントローラー
/// </summary>
class AICon_KaniArm : public IBodyController<BP_KaniArm>
	//TODO もう一個、ムーブセットのインターフェースクラスを継承させる?
{
public:
	using IBodyController::IBodyController;	
	void Update()override;
	//TODO
	//どこどこへの移動、相手への攻撃などムーブセットを実装する仮想関数作る?
private:
};
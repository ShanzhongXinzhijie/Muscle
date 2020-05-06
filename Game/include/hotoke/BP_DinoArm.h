#pragma once
#include "BodyPartsHeader.h"

/// <summary>
/// 恐竜腕
/// </summary>
class BP_DinoArm :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()const override {
		return new BP_DinoArm;
	}

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void Draw2D()override;

	//アクション
	void ChargeAndMachinegun(enLR);
	void Rocket(enLR);

	//チャージを取得
	int GetChargeTimer(enLR lr)const { return m_chargeTime[lr]; }

private:
	//コントローラー
	IBodyController<BP_DinoArm>* m_controller = nullptr;

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
	std::unique_ptr<GameSE> m_chargeSE[enLRNUM];
	static constexpr int MACHINE_GUN_CHARGE_TIME = 10, LAZER_CHARGE_TIME = MACHINE_GUN_CHARGE_TIME - 3;
	//クールダウン時間
	int m_coolDown[enLRNUM] = {};
	static constexpr int ROCKET_COOLDOWN = FRAME_RATE * 8;

	//HUD
	CSprite m_guncross;
	float m_angle[enLRNUM];
};

/// <summary>
/// 恐竜腕 #人コントローラー
/// </summary>
class HCon_DinoArm : public IBodyController<BP_DinoArm> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};

/// <summary>
/// 恐竜腕 #AIコントローラー
/// </summary>
class AICon_DinoArm : public IBodyController<BP_DinoArm>
{
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
private:
	int m_cnt = 0;
};
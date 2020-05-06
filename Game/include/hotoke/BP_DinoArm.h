#pragma once
#include "BodyPartsHeader.h"

/// <summary>
/// �����r
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

	//�A�N�V����
	void ChargeAndMachinegun(enLR);
	void Rocket(enLR);

	//�`���[�W���擾
	int GetChargeTimer(enLR lr)const { return m_chargeTime[lr]; }

private:
	//�R���g���[���[
	IBodyController<BP_DinoArm>* m_controller = nullptr;

	//�{�[���֌W
	AnimationClip m_initPose;
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};
	CVector3 m_ikTargetPos[enLRNUM] = {};
	int m_muzzleBoneID[enLRNUM] = {};

	//�G�t�F�N�g
	CBillboard m_muzzleFlash[enLRNUM];
	int m_muzzleTime[enLRNUM] = {};

	//�`���[�W�^�C�}�[
	bool m_isCharging[enLRNUM] = {}, m_isMachineGunning[enLRNUM] = {};
	int m_chargeTime[enLRNUM] = {};
	std::unique_ptr<GameSE> m_chargeSE[enLRNUM];
	static constexpr int MACHINE_GUN_CHARGE_TIME = 10, LAZER_CHARGE_TIME = MACHINE_GUN_CHARGE_TIME - 3;
	//�N�[���_�E������
	int m_coolDown[enLRNUM] = {};
	static constexpr int ROCKET_COOLDOWN = FRAME_RATE * 8;

	//HUD
	CSprite m_guncross;
	float m_angle[enLRNUM];
};

/// <summary>
/// �����r #�l�R���g���[���[
/// </summary>
class HCon_DinoArm : public IBodyController<BP_DinoArm> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};

/// <summary>
/// �����r #AI�R���g���[���[
/// </summary>
class AICon_DinoArm : public IBodyController<BP_DinoArm>
{
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
private:
	int m_cnt = 0;
};
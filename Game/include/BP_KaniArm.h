#pragma once
#include "BodyPartsHeader.h"

/// <summary>
/// �J�j�r
/// </summary>
class BP_KaniArm :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	
	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;

	//�A�N�V����
	void ChargeAndMachinegun(enLR);
	void Rocket(enLR);
	void Lazer(enLR);
	void Stab();

	//�`���[�W���擾
	int GetChargeTimer(enLR lr)const { return m_chargeTime[lr]; }

private:
	//�R���g���[���[
	IBodyController<BP_KaniArm>* m_controller = nullptr;

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
	static constexpr int MACHINE_GUN_CHARGE_TIME = 10, LAZER_CHARGE_TIME = MACHINE_GUN_CHARGE_TIME - 3;
};

/// <summary>
/// �J�j�r #�l�R���g���[���[
/// </summary>
class HCon_KaniArm : public IBodyController<BP_KaniArm> {
public:
	using IBodyController::IBodyController;	
	void Update()override;
};

/// <summary>
/// �J�j�r #AI�R���g���[���[
/// </summary>
class AICon_KaniArm : public IBodyController<BP_KaniArm>
	//TODO ������A���[�u�Z�b�g�̃C���^�[�t�F�[�X�N���X���p��������?
{
public:
	using IBodyController::IBodyController;	
	void Update()override;
	//TODO
	//�ǂ��ǂ��ւ̈ړ��A����ւ̍U���Ȃǃ��[�u�Z�b�g���������鉼�z�֐����?
private:
};
#pragma once
#include "IBodyPart.h"

class ICon_KaniArm;

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
	ICon_KaniArm* m_controller = nullptr;

	//�{�[���֌W
	AnimationClip m_initPose;
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};
	int m_muzzleBoneID[enLRNUM] = {};

	//�G�t�F�N�g
	CBillboard m_muzzleFlash[enLRNUM]; int m_muzzleTime[enLRNUM] = {};

	//�`���[�W�^�C�}�[
	bool m_isCharging[enLRNUM] = {}, m_isMachineGunning[enLRNUM] = {};
	int m_chargeTime[enLRNUM] = {};
	static constexpr int MACHINE_GUN_CHARGE_TIME = 10, LAZER_CHARGE_TIME = MACHINE_GUN_CHARGE_TIME - 3;
};

//TODO ����e���v���[�g��?
class ICon_KaniArm {
public:
	ICon_KaniArm(BP_KaniArm* ptrbody, CDeathHotoke* ptrCore): m_ptrBody(ptrbody), m_ptrCore(ptrCore){}
	virtual void Update() = 0;

protected:
	BP_KaniArm* m_ptrBody = nullptr;
	CDeathHotoke* m_ptrCore = nullptr;
};

class HCon_KaniArm : public ICon_KaniArm {
public:
	using ICon_KaniArm::ICon_KaniArm;
	
	void Update()override;
private:
};

class AICon_KaniArm : 
	public ICon_KaniArm
	//TODO ������A���[�u�Z�b�g�̃C���^�[�t�F�[�X�N���X���p��������?
{
public:
	using ICon_KaniArm::ICon_KaniArm;
	
	void Update()override;
	//TODO
	//�ǂ��ǂ��ւ̈ړ��A����ւ̍U���Ȃǃ��[�u�Z�b�g���������鉼�z�֐����?
private:
};
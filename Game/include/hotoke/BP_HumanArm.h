#pragma once
#include "BodyPartsHeader.h"

/// <summary>
/// �q�g�r
/// </summary>
class BP_HumanArm :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_HumanArm;
	}

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void Draw2D()override;

private:
	//�R���g���[���[
	IBodyController<BP_HumanArm>* m_controller = nullptr;

	//�{�[���֌W
	AnimationClip m_initPose;
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};
	CVector3 m_ikTargetPos[enLRNUM] = {};
	int m_muzzleBoneID[enLRNUM] = {};
};


/// <summary>
/// �q�g�r #�l�R���g���[���[
/// </summary>
class HCon_HumanArm : public IBodyController<BP_HumanArm> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override {};
};

/// <summary>
/// �q�g�r #AI�R���g���[���[
/// </summary>
class AICon_HumanArm : public IBodyController<BP_HumanArm>
{
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override {};
};
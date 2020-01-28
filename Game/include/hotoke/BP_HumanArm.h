#pragma once
#include "BodyPartsHeader.h"

/// <summary>
/// ヒト腕
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
	//コントローラー
	IBodyController<BP_HumanArm>* m_controller = nullptr;

	//ボーン関係
	AnimationClip m_initPose;
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};
	CVector3 m_ikTargetPos[enLRNUM] = {};
	int m_muzzleBoneID[enLRNUM] = {};
};


/// <summary>
/// ヒト腕 #人コントローラー
/// </summary>
class HCon_HumanArm : public IBodyController<BP_HumanArm> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override {};
};

/// <summary>
/// ヒト腕 #AIコントローラー
/// </summary>
class AICon_HumanArm : public IBodyController<BP_HumanArm>
{
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override {};
};
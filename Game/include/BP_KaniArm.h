#pragma once
#include "IBodyPart.h"
class BP_KaniArm :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	
	void InnerStart()override;
	void Update()override;

	//アクション
	void Charge();
	void MachineGun();
	void Rocket();
	void Lazer();	
	void Stab();

private:
	AnimationClip m_initPose;
	SkeletonIK::IKSetting* m_ikSetting[2] = {};

	int m_muzzleBoneID[2] = {};

	//エフェクト
	CBillboard m_muzzleFlash; int m_muzzleCnt = 0;
};


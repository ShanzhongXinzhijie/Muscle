#pragma once
#include "IBodyPart.h"
class BP_KaniArm :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	
	void InnerStart()override;
	void PostUTRSUpdate()override;

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

	float m_chargeTime = 0.0f;
};

class ICon_KaniArm {
public:
	ICon_KaniArm(BP_KaniArm* ptrbody): m_ptrBody(ptrbody){}
	virtual void Update() = 0;

private:
	BP_KaniArm* m_ptrBody = nullptr;
};

class HCon_KaniArm : public ICon_KaniArm {
public:
	void Update()override;
private:
};
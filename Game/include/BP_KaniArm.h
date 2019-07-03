#pragma once
#include "IBodyPart.h"
class BP_KaniArm :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	
	void InnerStart()override;
	void Update()override;

private:
	AnimationClip m_initPose;

	//エフェクト
	CBillboard m_muzzleFlash;
	Shader m_ps;
};


#pragma once
#include "IBodyPart.h"

class BP_HumanLeg :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;

	void InnerStart()override;
	void Update()override;

private:
	AnimationClip m_initPose;
	SkeletonIK::IKSetting* m_ikSetting[2];
};


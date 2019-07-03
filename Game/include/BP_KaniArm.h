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

	//�G�t�F�N�g
	CBillboard m_muzzleFlash;
	Shader m_ps;
};


#pragma once
#include "BodyPartsHeader.h"

class BP_HumanLeg :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;

private:
	AnimationClip m_initPose;							//�����|�[�Y
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};	//IK�ݒ�
	DHCollision m_col[enLRNUM];							//�R���W����
};


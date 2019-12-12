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

	//�W�����v
	void Jump();
	//���݂�
	void Stomp();
	//����
	//void Unchi();

private:
	//�R���g���[���[
	IBodyController<BP_HumanLeg>* m_controller = nullptr;

	AnimationClip m_initPose;							//�����|�[�Y
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};	//IK�ݒ�
	DHCollision m_col[enLRNUM];							//�R���W����
	bool m_isJump = false;
};

/// <summary>
/// �l�� #�l�R���g���[���[
/// </summary>
class HCon_HumanLeg : public IBodyController<BP_HumanLeg> {
public:
	using IBodyController::IBodyController;
	void Update()override;
};

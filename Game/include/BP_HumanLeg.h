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
	void PostLoopUpdate()override;
	void Draw2D()override;

	//�W�����v
	void Jump();
	//���݂�
	bool CanKick()const;
	void Stomp();
	//����
	//void Unchi();

private:
	//�R���g���[���[
	IBodyController<BP_HumanLeg>* m_controller = nullptr;

	AnimationClip m_initPose;							//�����|�[�Y
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};	//IK�ݒ�
	CVector3 m_beforePos[enLRNUM];
	DHCollision m_col[enLRNUM];							//�R���W����
	bool m_isJump = false;								//�W�����v����t���O
	int m_leftStomp = 1;								//�c�蓥�݂���
};

/// <summary>
/// �l�� #�l�R���g���[���[
/// </summary>
class HCon_HumanLeg : public IBodyController<BP_HumanLeg> {
public:
	using IBodyController::IBodyController;
	void Update()override;
};

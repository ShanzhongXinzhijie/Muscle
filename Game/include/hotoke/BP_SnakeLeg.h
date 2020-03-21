#pragma once
#include "BodyPartsHeader.h"

class BP_SnakeLeg :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()const override {
		return new BP_SnakeLeg;
	}

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void Draw2D()override;

	void Dash();

private:
	//�R���g���[���[
	IBodyController<BP_SnakeLeg>* m_controller = nullptr;

	//�A�j���[�V����
	AnimationClip m_animation[1];

	CVector3 m_beforePos;//�O��̈ʒu
	DHCollision m_col;//�R���W����

	SkeletonIK::IKSetting* m_ikSetting[2];
	Bone* m_legBone = nullptr;

	bool m_isDash = false;//�_�b�V���t���O
	bool m_canDash = false;
};

/// <summary>
/// �֑� #�l�R���g���[���[
/// </summary>
class HCon_SnakeLeg : public IBodyController<BP_SnakeLeg> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};


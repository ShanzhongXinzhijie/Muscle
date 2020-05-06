#pragma once
#include "BodyPartsHeader.h"

class BP_TankLeg :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()const override {
		return new BP_TankLeg;
	}

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void Draw2D()override;	

	void Accel();
	void Turn(enLR lr);

private:
	//�R���g���[���[
	IBodyController<BP_TankLeg>* m_controller = nullptr;

	CVector3 m_beforePos;//�O��̈ʒu
	DHCollision m_col;//�R���W����

	Bone* m_eye = nullptr;//���_�ʒu�{�[��
	Bone* m_legs[6][2] = {};//���{�[��
	int m_legHitNum = 0;//�ڒn���Ă��鑫�̐�

	//���s�A�N�V����
	enum enAction {
		enNone,
		enAccel,enTurnL,enTurnR,
		enActionNum
	};
	enAction m_actionType = enNone;

	//SE
	std::unique_ptr<GameSE> m_tankSE;

	//���_�ʒu
	float m_viewFront = 158.0f, m_viewHeight = -28.0f;//152
};

/// <summary>
/// ��ԑ� #�l�R���g���[���[
/// </summary>
class HCon_TankLeg : public IBodyController<BP_TankLeg> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};


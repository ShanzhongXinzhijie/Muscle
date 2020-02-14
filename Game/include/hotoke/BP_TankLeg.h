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

private:
	//�R���g���[���[
	IBodyController<BP_TankLeg>* m_controller = nullptr;

	CVector3 m_beforePos;//�O��̈ʒu
	DHCollision m_col;//�R���W����

	Bone* m_eye = nullptr;//���_�ʒu�{�[��
	Bone* m_legs[6][2] = {};//���{�[��
};

/// <summary>
/// ��ԑ� #�l�R���g���[���[
/// </summary>
class HCon_TankLeg : public IBodyController<BP_TankLeg> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};


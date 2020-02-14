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
	//コントローラー
	IBodyController<BP_TankLeg>* m_controller = nullptr;

	CVector3 m_beforePos;//前回の位置
	DHCollision m_col;//コリジョン

	Bone* m_eye = nullptr;//視点位置ボーン
	Bone* m_legs[6][2] = {};//足ボーン
};

/// <summary>
/// 戦車足 #人コントローラー
/// </summary>
class HCon_TankLeg : public IBodyController<BP_TankLeg> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};


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
	//コントローラー
	IBodyController<BP_TankLeg>* m_controller = nullptr;

	CVector3 m_beforePos;//前回の位置
	DHCollision m_col;//コリジョン

	Bone* m_eye = nullptr;//視点位置ボーン
	Bone* m_legs[6][2] = {};//足ボーン
	int m_legHitNum = 0;//接地している足の数

	//実行アクション
	enum enAction {
		enNone,
		enAccel,enTurnL,enTurnR,
		enActionNum
	};
	enAction m_actionType = enNone;

	//SE
	std::unique_ptr<GameSE> m_tankSE;

	//視点位置
	float m_viewFront = 158.0f, m_viewHeight = -28.0f;//152
};

/// <summary>
/// 戦車足 #人コントローラー
/// </summary>
class HCon_TankLeg : public IBodyController<BP_TankLeg> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};


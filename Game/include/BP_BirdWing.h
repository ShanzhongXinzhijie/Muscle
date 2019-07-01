#pragma once
#include "IBodyPart.h"
class BP_BirdWing :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;

	void InnerStart()override;
	void Update()override;
	void Draw2D()override;

private:
	//SuicideObj::CCollisionObj m_col;

	float m_accel = 0.0f;

	//‰ñ“]“ü—ÍŠÖŒW
	float m_inputTimer = 0.0f;//“ü—Í—P—\
	int m_spinCount = 0, m_spinCountBefore = 0;//‰ñ“]ƒJƒEƒ“ƒg
	enum enSpinDirection{
		enUp, enRight, enDown, enLeft,
		enDirNum,
	};
	enSpinDirection m_nextDirection = enDirNum;//Ÿ‚É“ü—Í‚·‚é‰ñ“]
	enSpinDirection m_backDirection = enDirNum;//Ÿ‚É“ü—Í‚·‚é‹t‰ñ“]
	enum enSpinMode {
		enNone,enClockwise,enBack,
	};
	enSpinMode m_spinMode = enNone;//‰ñ“]ƒ‚[ƒh
};


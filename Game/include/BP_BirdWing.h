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

	//��]���͊֌W
	float m_inputTimer = 0.0f;//���͗P�\
	int m_spinCount = 0, m_spinCountBefore = 0;//��]�J�E���g
	enum enSpinDirection{
		enUp, enRight, enDown, enLeft,
		enDirNum,
	};
	enSpinDirection m_nextDirection = enDirNum;//���ɓ��͂����]
	enSpinDirection m_backDirection = enDirNum;//���ɓ��͂���t��]
	enum enSpinMode {
		enNone,enClockwise,enBack,
	};
	enSpinMode m_spinMode = enNone;//��]���[�h
};


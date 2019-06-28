#pragma once
#include "IBodyPart.h"
class BirdWing :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;

	void Start()override;
	void Update()override;
	void Draw2D()override;

private:
	void SetTRS();

private:
	GameObj::CSkinModelRender m_model;
	//SuicideObj::CCollisionObj m_col;

	float m_accel = 0.0f;

	//��]���͊֌W
	float m_inputTimer = 0.0f;//���͗P�\
	int m_spinCount = 0;//��]�J�E���g
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


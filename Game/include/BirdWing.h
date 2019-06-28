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

	//回転入力関係
	float m_inputTimer = 0.0f;//入力猶予
	int m_spinCount = 0;//回転カウント
	enum enSpinDirection{
		enUp, enRight, enDown, enLeft,
		enDirNum,
	};
	enSpinDirection m_nextDirection = enDirNum;//次に入力する回転
	enSpinDirection m_backDirection = enDirNum;//次に入力する逆回転
	enum enSpinMode {
		enNone,enClockwise,enBack,
	};
	enSpinMode m_spinMode = enNone;//回転モード
};


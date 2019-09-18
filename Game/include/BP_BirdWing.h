#pragma once
#include "BodyPartsHeader.h"
class BP_BirdWing :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;

	void InnerStart()override;
	void Update()override;
	void Draw2D()override;

	//アクション
	void Accel();
	void Brake();
	void Pitch(float lerp);//上下回転 lerp:回転量(0.0f～1.0f)
	void Yaw(float lerp);//左右回転 lerp:回転量(0.0f～1.0f)

	float GetAccel()const { return m_accel; }

private:
	//コントローラー
	IBodyController<BP_BirdWing>* m_controller = nullptr;

	float m_accel = 0.0f;//加速
	float m_yawAccel = 0.0f;
	CQuaternion m_pitch;//縦回転
};

/// <summary>
/// 鳥羽 #人コントローラー
/// </summary>
class HCon_BirdWing : public IBodyController<BP_BirdWing> {
public:
	using IBodyController::IBodyController;
	void Update()override;
private:
	float m_accelTime = 0.0f;
	int m_beforeClrcleInputNum = 0;
};
/// <summary>
/// 鳥羽 #AIコントローラー
/// </summary>
class AICon_BirdWing : public IBodyController<BP_BirdWing> {
public:
	using IBodyController::IBodyController;
	void Update()override;
};
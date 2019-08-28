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

	//ANV
	void Accel();
	void Brake();
	void Pitch(float lerp);//ãºñ] lerp:ñ]Ê(0.0f`1.0f)
	void Yaw(float lerp);//¶Eñ] lerp:ñ]Ê(0.0f`1.0f)

	float GetAccel()const { return m_accel; }

private:
	//Rg[[
	IBodyController<BP_BirdWing>* m_controller = nullptr;

	float m_accel = 0.0f;//Á¬
	float m_yawAccel = 0.0f;
	CQuaternion m_pitch;//cñ]
};

/// <summary>
/// ¹H #lRg[[
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
/// ¹H #AIRg[[
/// </summary>
class AICon_BirdWing : public IBodyController<BP_BirdWing> {
public:
	using IBodyController::IBodyController;
	void Update()override;
};
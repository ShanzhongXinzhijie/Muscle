#pragma once
#include "BodyPartsHeader.h"
class BP_BirdWing :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()const override {
		return new BP_BirdWing;
	}

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
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

	//Aj[V
	enum enAnim{
		enDefault,enFlying,enBraking,enAnimNum,
	};
	AnimationClip m_anim[enAnimNum];
	bool m_isAnimEnd = true;
	bool m_isBraking = false;
	//Bone* m_shoulderBone[enLRNUM] = {};

	float m_accel = 0.0f;//Á¬
	float m_yawAccel = 0.0f; bool m_isYawInput = false;
	float m_pitchAccel = 0.0f; bool m_isPitchInput = false;
	CQuaternion m_pitch;//cñ]

	//float m_y = 0;
};

/// <summary>
/// ¹H #lRg[[
/// </summary>
class HCon_BirdWing : public IBodyController<BP_BirdWing> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
private:
	float m_accelTime = 0.0f;
	int m_beforeClrcleInputNum = 0;
};
/// <summary>
/// ¹H #AIRg[[
/// </summary>
class AICon_BirdWing : public IBodyController<BP_BirdWing> {
public:
	//using IBodyController::IBodyController;
	AICon_BirdWing(BP_BirdWing* ptrbody, CDeathHotoke* ptrCore);

	void InnerUpdate()override;

private:
	int m_count = 0;
	bool m_isHardAI = true;
	bool m_isDownMode = false;
	float m_downPower = 0.5f;
};
#pragma once
#include "IBodyPart.h"

class ICon_KaniArm;

class BP_KaniArm :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	
	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;

	//アクション
	void Charge();
	void MachineGun();
	void Rocket();
	void Lazer();	
	void Stab();

private:
	//コントローラー
	ICon_KaniArm* m_controller = nullptr;

	AnimationClip m_initPose;
	SkeletonIK::IKSetting* m_ikSetting[2] = {};

	int m_muzzleBoneID[2] = {};

	//エフェクト
	CBillboard m_muzzleFlash; int m_muzzleCnt = 0;

	//チャージタイマー
	bool m_isCharging = false;
	float m_chargeTime = 0.0f;
	static constexpr float m_machineGunTime = 24.0f;
};

//TODO これテンプレート化?
class ICon_KaniArm {
public:
	ICon_KaniArm(BP_KaniArm* ptrbody, CDeathHotoke* ptrCore): m_ptrBody(ptrbody), m_ptrCore(ptrCore){}
	virtual void Update() = 0;

protected:
	BP_KaniArm* m_ptrBody = nullptr;
	CDeathHotoke* m_ptrCore = nullptr;
};

class HCon_KaniArm : public ICon_KaniArm {
public:
	using ICon_KaniArm::ICon_KaniArm;
	
	void Update()override;
private:
};

class AICon_KaniArm : 
	public ICon_KaniArm
	//TODO もう一個、ムーブセットのインターフェースクラスを継承させる?
{
public:
	using ICon_KaniArm::ICon_KaniArm;
	
	void Update()override;
	//TODO
	//どこどこへの移動、相手への攻撃などムーブセットを実装する仮想関数作る?
private:
};
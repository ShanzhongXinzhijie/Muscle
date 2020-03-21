#pragma once
#include "BodyPartsHeader.h"

class BP_SnakeLeg :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()const override {
		return new BP_SnakeLeg;
	}

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void Draw2D()override;

	void Dash();

private:
	//コントローラー
	IBodyController<BP_SnakeLeg>* m_controller = nullptr;

	//アニメーション
	AnimationClip m_animation[1];

	CVector3 m_beforePos;//前回の位置
	DHCollision m_col;//コリジョン

	SkeletonIK::IKSetting* m_ikSetting[2];
	Bone* m_legBone = nullptr;

	bool m_isDash = false;//ダッシュフラグ
	bool m_canDash = false;
};

/// <summary>
/// 蛇足 #人コントローラー
/// </summary>
class HCon_SnakeLeg : public IBodyController<BP_SnakeLeg> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};


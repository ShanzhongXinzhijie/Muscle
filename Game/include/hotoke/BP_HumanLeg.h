#pragma once
#include "BodyPartsHeader.h"

class BP_HumanLeg :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()const override {
		return new BP_HumanLeg;
	}

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void PostLoopUpdate()override;
	void Draw2D()override;

	//ジャンプ
	void Jump();
	//踏みつけ
	bool CanKick()const;
	void Stomp();

private:
	//コントローラー
	IBodyController<BP_HumanLeg>* m_controller = nullptr;

	AnimationClip m_initPose;							//初期ポーズ
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};	//IK設定
	CVector3 m_beforePos[enLRNUM];						//各足の前フレーム座標
	DHCollision m_col[enLRNUM];							//コリジョン
	bool m_isJump = false;								//ジャンプするフラグ
	int m_leftStomp = 1;								//残り踏みつけ回数
	bool m_isOnGraund = false;							//設置しているか	
};

/// <summary>
/// 人足 #人コントローラー
/// </summary>
class HCon_HumanLeg : public IBodyController<BP_HumanLeg> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};

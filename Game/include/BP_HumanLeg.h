#pragma once
#include "BodyPartsHeader.h"

class BP_HumanLeg :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;

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
	//うんち
	//void Unchi();

private:
	//コントローラー
	IBodyController<BP_HumanLeg>* m_controller = nullptr;

	AnimationClip m_initPose;							//初期ポーズ
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};	//IK設定
	CVector3 m_beforePos[enLRNUM];
	DHCollision m_col[enLRNUM];							//コリジョン
	bool m_isJump = false;								//ジャンプするフラグ
	int m_leftStomp = 1;								//残り踏みつけ回数
};

/// <summary>
/// 人足 #人コントローラー
/// </summary>
class HCon_HumanLeg : public IBodyController<BP_HumanLeg> {
public:
	using IBodyController::IBodyController;
	void Update()override;
};

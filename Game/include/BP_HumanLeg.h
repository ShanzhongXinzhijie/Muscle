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

	//ジャンプ
	void Jump();
	//踏みつけ
	void Stomp();
	//うんち
	void Unchi();

private:
	AnimationClip m_initPose;							//初期ポーズ
	SkeletonIK::IKSetting* m_ikSetting[enLRNUM] = {};	//IK設定
	DHCollision m_col[enLRNUM];							//コリジョン
};


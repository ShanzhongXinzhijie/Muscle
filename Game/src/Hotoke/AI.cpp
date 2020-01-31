#include "stdafx.h"
#include "AI.h"
#include "CDeathHotoke.h"

void TestAI::Update() {
	//ターゲットを探す
	CDeathHotoke* target = nullptr; float distance = -1.0f;
	QueryGOs<CDeathHotoke>(L"CDeathHotoke",
		[&](auto* P) {
			if (P == m_ptrCore) { return true; }//自分
			if (distance < 0.0f || distance > (P->GetPos()- m_ptrCore->GetPos()).LengthSq()) {//いちばんきょりがちかいものがもくひょう
				distance = (P->GetPos() - m_ptrCore->GetPos()).LengthSq();
				target = P;
			}
			return false;//クエリ終了
		}
	);	

	if (target) {
		//敵の位置を目指して移動
		m_outputStatus.moveTargetPosition = target->GetPos();
		m_outputStatus.isMovingToTarget = true;
		//敵の位置を攻撃
		m_ptrCore->SetTargetPos(target->GetPos());
		m_ptrCore->SetTarget(target->GetLockableWrapper());
		m_outputStatus.isAttackingTarget = true;
	}
}
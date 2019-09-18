#include "stdafx.h"
#include "AI.h"
#include "CDeathHotoke.h"

void TestAI::Update() {
	QueryGOs<CDeathHotoke>(L"CDeathHotoke",
		[&](auto* P) {
			if (P == m_ptrCore) { return true; }//自分
			//敵の位置を目指して移動
			m_outputStatus.moveTargetPosition = P->GetPos();
			m_outputStatus.isMovingToTarget = true;
			//敵の位置を攻撃
			m_ptrCore->SetTargetPos(P->GetPos());
			m_outputStatus.isAttackingTarget = true;
			return false;//クエリ終了
		}
	);	
}
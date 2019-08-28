#include "stdafx.h"
#include "AI.h"
#include "CDeathHotoke.h"

void TestAI::Update() {
	QueryGOs<CDeathHotoke>(L"CDeathHotoke",
		[&](auto* P) {
			if (P == m_ptrCore) { return true; }//自分
			m_outputStatus.moveTargetPosition = P->GetPos();//敵の位置を目指す
			m_outputStatus.isMovingToTarget = true;
			return false;//クエリ終了
		}
	);	
}
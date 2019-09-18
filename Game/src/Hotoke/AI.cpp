#include "stdafx.h"
#include "AI.h"
#include "CDeathHotoke.h"

void TestAI::Update() {
	QueryGOs<CDeathHotoke>(L"CDeathHotoke",
		[&](auto* P) {
			if (P == m_ptrCore) { return true; }//����
			//�G�̈ʒu��ڎw���Ĉړ�
			m_outputStatus.moveTargetPosition = P->GetPos();
			m_outputStatus.isMovingToTarget = true;
			//�G�̈ʒu���U��
			m_ptrCore->SetTargetPos(P->GetPos());
			m_outputStatus.isAttackingTarget = true;
			return false;//�N�G���I��
		}
	);	
}
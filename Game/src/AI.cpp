#include "stdafx.h"
#include "AI.h"
#include "CDeathHotoke.h"

void TestAI::Update() {
	QueryGOs<CDeathHotoke>(L"CDeathHotoke",
		[&](auto* P) {
			if (P == m_ptrCore) { return true; }//����
			m_outputStatus.moveTargetPosition = P->GetPos();//�G�̈ʒu��ڎw��
			m_outputStatus.isMovingToTarget = true;
			return false;//�N�G���I��
		}
	);	
}
#include "stdafx.h"
#include "AI.h"
#include "CDeathHotoke.h"

void TestAI::Update() {
	//�^�[�Q�b�g��T��
	CDeathHotoke* target = nullptr; float distance = -1.0f;
	QueryGOs<CDeathHotoke>(L"CDeathHotoke",
		[&](auto* P) {
			if (P == m_ptrCore) { return true; }//����
			if (distance < 0.0f || distance > (P->GetPos()- m_ptrCore->GetPos()).LengthSq()) {//�����΂񂫂�肪���������̂������Ђ傤
				distance = (P->GetPos() - m_ptrCore->GetPos()).LengthSq();
				target = P;
			}
			return false;//�N�G���I��
		}
	);	

	if (target) {
		//�G�̈ʒu��ڎw���Ĉړ�
		m_outputStatus.moveTargetPosition = target->GetPos();
		m_outputStatus.isMovingToTarget = true;
		//�G�̈ʒu���U��
		m_ptrCore->SetTargetPos(target->GetPos());
		m_ptrCore->SetTarget(target->GetLockableWrapper());
		m_outputStatus.isAttackingTarget = true;
	}
}
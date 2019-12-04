#include "stdafx.h"
#include "IFu.h"

IFu::IFu() {
	//�����蔻��
	m_col.m_collision.SetIsHurtCollision(true);//����͋�炢����
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//�N���X���o��
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				if (H->m_preCollisionFunc) { H->m_preCollisionFunc(&m_col.m_reference); }
				if (m_collisionFunc) { m_collisionFunc(H, p); }
			}
		}
	);
}
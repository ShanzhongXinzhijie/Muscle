#include "stdafx.h"
#include "IFu.h"

IFu::IFu() {
	//“–‚½‚è”»’è
	m_col.m_collision.SetIsHurtCollision(true);//‚±‚ê‚Í‹ò‚ç‚¢”»’è
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//ƒNƒ‰ƒXæ‚èo‚·
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				if (H->m_preCollisionFunc) { H->m_preCollisionFunc(&m_col.m_reference); }
				if (m_collisionFunc) { m_collisionFunc(H, p); }
			}
		}
	);
}
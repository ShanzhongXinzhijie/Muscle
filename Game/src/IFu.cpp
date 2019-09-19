#include "stdafx.h"
#include "IFu.h"

IFu::IFu() {
	//当たり判定
	m_col.m_collision.SetIsHurtCollision(true);//これは喰らい判定
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//クラス取り出す
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				m_collisionFunc(H);
			}
		}
	);
}
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
				//相手の衝突前処理
				if (H->m_preCollisionFunc) { 
					if (H->m_preCollisionFunc(&m_col.m_reference) == false) {
						return;//衝突しない
					}
				}
				//自分の衝突時処理
				if (m_collisionFunc) {
					m_collisionFunc(H, p);
				}
			}
		}
	);
}
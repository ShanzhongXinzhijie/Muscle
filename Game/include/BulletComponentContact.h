#pragma once
#include<unordered_set>
#include"BulletKani.h"

/// <summary>
/// 衝突で消滅コンポーネント
/// </summary>
class BD_Contact : public IBulletComponent {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="isContactField">地面と衝突するか?</param>
	BD_Contact(bool isContactField = true) : m_isContactField(isContactField) {}

	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//クラス取り出す
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical] && (m_isContactField || !H->attributes[enGraund])) {
				//物理属性なら死
				m_bullet->m_lifeTime = 0.0f;
				m_bullet->SetPos(p.m_collisionPoint);
			}
		}
		if (!p.m_isCCollisionObj) {
			//相手がCCollisionObjじゃなくても死
			m_bullet->m_lifeTime = 0.0f;
			m_bullet->SetPos(p.m_collisionPoint);
		}
	}
private:
	bool m_isContactField = true;
};
/// <summary>
/// 跳弾コンポーネント
/// </summary>
class BD_Reflect : public IBulletComponent {
public:
	void PostUpdate()override {
		//レイで判定
		btVector3 rayStart = m_bullet->GetOldPos();
		btVector3 rayEnd = m_bullet->GetPos();
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			CVector3 Nvec = m_bullet->m_vector.GetNorm();
			//入射ベクトル多少ランダムに
			CVector3 randamAxis = { CMath::RandomZeroToOne() - 0.5f,CMath::RandomZeroToOne() - 0.5f ,CMath::RandomZeroToOne() - 0.5f };
			randamAxis.Normalize();
			CQuaternion randamRot(randamAxis, CMath::RandomZeroToOne()*0.5f);
			randamRot.Multiply(Nvec);
			//反射ベクトル
			m_bullet->m_vector = CMath::CalcReflectVector(Nvec, gnd_ray.m_hitNormalWorld) * m_bullet->m_vector.Length();
			m_bullet->SetPos(gnd_ray.m_hitPointWorld + gnd_ray.m_hitNormalWorld * (m_bullet->m_radius + 1.0f));
		}
	}
};
/// <summary>
/// 衝突で爆発コンポーネント
/// </summary>
class BD_ContactExplosion : public IBulletComponent {
public:
	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//クラス取り出す
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical]) {
				//物理属性なら死
				m_bullet->m_lifeTime = 0.0f;
				m_bullet->SetPos(p.m_collisionPoint);
				//TODO 爆発生成
			}
		}
		if (!p.m_isCCollisionObj) {
			//相手がCCollisionObjじゃなくても死
			m_bullet->m_lifeTime = 0.0f;
			m_bullet->SetPos(p.m_collisionPoint);
			//TODO 爆発生成
		}
	}
};
/// <summary>
/// 一回衝突したものとは衝突しないコンポーネント
/// </summary>
class BD_OneContactMask : public IBulletComponent {
public:
	bool PreContact(ReferenceCollision* refcol)override {
		if (m_contactedObjects.find(refcol) == m_contactedObjects.end()) {
			//一度も衝突してない
			m_contactedObjects.emplace(refcol);
			return true;
		}
		//すでに衝突済み
		return false;
	}
private:
	std::unordered_set<ReferenceCollision*> m_contactedObjects;
};
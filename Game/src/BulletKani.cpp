#include "stdafx.h"
#include "BulletKani.h"
#include "CShockRing.h"

namespace {
	constexpr int BOOST_EFECT_MAXNUM = 512;
}

BulletGO::BulletGO(const CVector3& pos, const CVector3& move, IFu* owner, float damege, bool isLockable, int priorityLevel)
	: m_vector(move), m_owner(owner), ILifeObject(isLockable, owner)
{
	//初期座標
	SetPos(pos), m_posOld = GetPos();
	m_defaultPos = GetPos();

	//ロックオン優先度
	if (isLockable) {
		GetLockableWrapper()->SetPriorityLevel(priorityLevel);
	}
	
	//攻撃判定
	//ID設定
	m_col.m_reference.ownerID = GetFuID();
	m_col.m_reference.nonHitID = owner->GetFuID();
	//衝突処理
	m_col.SetCollisionCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			for (auto& component : m_components) {
				if (!component->GetEnable()) { continue; }
				component->Contact(p);
			}		
		}
	);
	//衝突前処理
	m_col.m_reference.m_preCollisionFunc = 
		[&](ReferenceCollision* refcol) {
			bool isContact = true;
			for (auto& component : m_components) {
				if (!component->GetEnable()) { continue; }
				if (component->PreContact(refcol) == false) {
					isContact = false;
				}
			}
			return isContact;
	};
	m_col.m_collision.SetIsCollisionStaticObject(true);//静的オブジェクトとも衝突する
	m_col.m_collision.SetIsHighSpeed(true);//これは高速です

	//ダメージ設定
	m_col.m_reference.damege = damege;
	m_defaultDamege = damege;

	//属性設定
	//TODO
	m_col.m_reference.attributes.set(enPhysical);
	m_col.m_reference.attributes.set(enFlame);

	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->Start();
	}
}

void BulletGO::Update() {
	//m_isMoved = true;

	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->Update();
	}

	//移動
	m_posOld = GetPos();
	Move(m_vector);

	//発射点からの高さひらく(落下する)ほど威力上昇
	if (m_isDamegeHeightBoost) {
		float power = max(0.0f, m_defaultPos.y - GetPos().y) / (LIMIT_HEIGHT_METER * METER);
		m_col.m_reference.damege = m_defaultDamege * (1.0f + power);

		//エフェクト
		if (!m_damegeBoostEffect) {
			m_damegeBoostEffect = std::make_unique<CBillboard>();
			m_damegeBoostEffect->Init(L"Resource/effect/light.png", BOOST_EFECT_MAXNUM);
			m_damegeBoostEffect->SetRot({ CVector3::AxisZ(), CMath::RandomZeroToOne()*CMath::PI2 });
			m_damegeBoostEffect->GetModel().InitPostDraw(PostDrawModelRender::enAlpha, false, GameObj::CSkinModelRender::enSoftParticle);//ポストドロー
			m_damegeBoostEffect->GetModel().SetIsShadowCaster(false);
		}
		m_damegeBoostEffect->SetPos(GetPos());
		m_damegeBoostEffect->SetScale(1500.0f*power);
	}

	//寿命処理
	m_lifeTime -= 1.0f;// FRAME_PER_SECOND;
	if (m_lifeTime < FLT_EPSILON) {
		new CShockRing(GetPos(), {0.f, 0.f, 0.f, 0.5f}, 400.0f);
		Death(); return;
	}

	//コリジョン更新
	m_col.SetPos(GetPos());
	m_col.SetVelocity(m_vector);

	//ベロシティ変更
	CalcVelocityUpdate(m_vector, m_gravity, m_upBrake, m_downAccel);

	for (auto& component : m_components) {
		if (!component->GetEnable()) {
			continue;
		}
		component->PostUpdate();
	}
}

void BulletGO::CalcVelocityUpdate(CVector3& velocity, float gravity, float upBrake, float downAccel) {
	//重力
	velocity.y -= gravity;

	if (velocity.y > 0.0f) {
		//上昇減速
		velocity = velocity.GetNorm() * (velocity.Length() + min(-velocity.y*upBrake, 0.0f));
	}
	else {
		//落下加速
		velocity = velocity.GetNorm() * (velocity.Length() + max(-velocity.y*downAccel, 0.0f));
	}
}

void BulletGO::PreLoopUpdate() {
	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->PreLoopUpdate();
	}
}

void BulletGO::PostLoopUpdate() {
	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->PostLoopUpdate();
	}
}

void BulletGO::Pre3DRender(int n) {
	for (auto& component : m_components) {
		if (!component->GetEnable()) { continue; }
		component->Pre3DRender(n);
	}
}

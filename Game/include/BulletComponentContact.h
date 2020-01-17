#pragma once
#include<unordered_set>
#include"BulletKani.h"

/// <summary>
/// �Փ˂ŏ��ŃR���|�[�l���g
/// </summary>
class BD_Contact : public IBulletComponent {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="isContactField">�n�ʂƏՓ˂��邩?</param>
	BD_Contact(bool isContactField = true) : m_isContactField(isContactField) {}

	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//�N���X���o��
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical] && (m_isContactField || !H->attributes[enGraund])) {
				//���������Ȃ玀
				m_bullet->m_lifeTime = 0.0f;
				m_bullet->SetPos(p.m_collisionPoint);
			}
		}
		if (!p.m_isCCollisionObj) {
			//���肪CCollisionObj����Ȃ��Ă���
			m_bullet->m_lifeTime = 0.0f;
			m_bullet->SetPos(p.m_collisionPoint);
		}
	}
private:
	bool m_isContactField = true;
};
/// <summary>
/// ���e�R���|�[�l���g
/// </summary>
class BD_Reflect : public IBulletComponent {
public:
	void PostUpdate()override {
		//���C�Ŕ���
		btVector3 rayStart = m_bullet->GetOldPos();
		btVector3 rayEnd = m_bullet->GetPos();
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			CVector3 Nvec = m_bullet->m_vector.GetNorm();
			//���˃x�N�g�����������_����
			CVector3 randamAxis = { CMath::RandomZeroToOne() - 0.5f,CMath::RandomZeroToOne() - 0.5f ,CMath::RandomZeroToOne() - 0.5f };
			randamAxis.Normalize();
			CQuaternion randamRot(randamAxis, CMath::RandomZeroToOne()*0.5f);
			randamRot.Multiply(Nvec);
			//���˃x�N�g��
			m_bullet->m_vector = CMath::CalcReflectVector(Nvec, gnd_ray.m_hitNormalWorld) * m_bullet->m_vector.Length();
			m_bullet->SetPos(gnd_ray.m_hitPointWorld + gnd_ray.m_hitNormalWorld * (m_bullet->m_radius + 1.0f));
		}
	}
};
/// <summary>
/// �Փ˂Ŕ����R���|�[�l���g
/// </summary>
class BD_ContactExplosion : public IBulletComponent {
public:
	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//�N���X���o��
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical]) {
				//���������Ȃ玀
				m_bullet->m_lifeTime = 0.0f;
				m_bullet->SetPos(p.m_collisionPoint);
				//TODO ��������
			}
		}
		if (!p.m_isCCollisionObj) {
			//���肪CCollisionObj����Ȃ��Ă���
			m_bullet->m_lifeTime = 0.0f;
			m_bullet->SetPos(p.m_collisionPoint);
			//TODO ��������
		}
	}
};
/// <summary>
/// ���Փ˂������̂Ƃ͏Փ˂��Ȃ��R���|�[�l���g
/// </summary>
class BD_OneContactMask : public IBulletComponent {
public:
	bool PreContact(ReferenceCollision* refcol)override {
		if (m_contactedObjects.find(refcol) == m_contactedObjects.end()) {
			//��x���Փ˂��ĂȂ�
			m_contactedObjects.emplace(refcol);
			return true;
		}
		//���łɏՓˍς�
		return false;
	}
private:
	std::unordered_set<ReferenceCollision*> m_contactedObjects;
};
#pragma once
#include"BeamModel.h"
#include"IFu.h"

class BulletGO;

/// <summary>
/// �o���b�g��
/// �z�[�~���O�E����O��
/// �ڐG�����E����
/// �����̕ӂ��f�R�邽�߂̂��
/// </summary>
class IBulletComponent {
public:
	//IBulletDecolator(IBulletDecolator* ptrDecolator = nullptr) :m_decolator(ptrDecolator) {}
	//virtual ~IBulletDecolator() { if (m_decolator) { delete m_decolator; m_decolator = nullptr; } }

	/*void DecoUpdate() {
		if (m_decolator)m_decolator->DecoUpdate();
		InnerUpdate();
	}
	void DecoPostUpdate() {
		if (m_decolator)m_decolator->DecoPostUpdate();
		InnerPostUpdate();
	}
	void DecoContact(SuicideObj::CCollisionObj::SCallbackParam& p) {
		if (m_decolator)m_decolator->DecoContact(p);
		InnerContact(p);
	}*/

	virtual void Update() {}
	virtual void PostUpdate()  {}
	virtual void Contact(SuicideObj::CCollisionObj::SCallbackParam& p) {}

	void SetBullet(BulletGO* bullet) {
		m_bullet = bullet;
		//if (m_decolator) {
		//	m_decolator->SetBullet(m_bullet);
		//}
	}
	/*BulletGO* GetBullet() {
		return m_bullet;		
	}*/

//private:
	//IBulletDecolator* m_decolator = nullptr;
protected:
	BulletGO* m_bullet = nullptr;
};

/// <summary>
/// �o���b�g�𓮂����N���X
/// </summary>
class BulletGO : public IQSGameObject, public IFu {
public:
	BulletGO(const CVector3& pos, const CVector3& move);
	//~BulletGO() { if (m_decolator) { delete m_decolator; m_decolator = nullptr; } }

	void Update()override;
	void PostLoopUpdate()override;

	/// <summary>
	/// �R���|�[�l���g��ǉ�����
	/// </summary>
	void AddComponent(std::unique_ptr<IBulletComponent> component) {
		m_components.emplace_back(std::move(component));
		m_components.back()->SetBullet(this);
	}

private:
	//�f�R���[�^�[
	//IBulletDecolator* m_decolator = nullptr;
	//�R���|�[�l���g
	std::list<std::unique_ptr<IBulletComponent>> m_components;
	
	//���f��
	BeamModel m_model;
	bool m_isMoved = false;
	CVector3 m_lastDrawPos;	

	//���W�Ƃ�
	CVector3 m_posOld;
	//CVector3 m_pos, m_posOld;

	//�R���W����
	DHCollision m_col;

public:
	//����
	float m_lifeTime = 3.0f;
	//�i�s�x�N�g��
	CVector3 m_vector;
};

/// <summary>
/// �z�[�~���O�f�R���[�^�[
/// </summary>
class BD_Homing : public IBulletComponent {
public:
	//TODO IFu�̎Q�Ɛ؂�
	BD_Homing(const IFu* target = nullptr) :m_target(target) {};

	void Update()override {
		if (!m_target) { return; }
		m_bullet->m_vector = (m_target->GetPos() - m_bullet->GetPos()).GetNorm()*m_bullet->m_vector.Length();
	}

private:
	const IFu* m_target = nullptr;
};

/// <summary>
/// �Փ˂ŏ��Ńf�R���[�^�[
/// </summary>
class BD_Contact : public IBulletComponent {
public:
	void Contact(SuicideObj::CCollisionObj::SCallbackParam& p)override {
		if (p.EqualName(L"ReferenceCollision")) {
			//�N���X���o��
			ReferenceCollision* H = p.GetClass<ReferenceCollision>();
			if (H->attributes[enPhysical]) {
				//���������Ȃ玀
				m_bullet->m_lifeTime = 0.0f;
			}
		}
		if (!p.m_isCCollisionObj) {
			//���肪CCollisionObj����Ȃ��Ă���
			m_bullet->m_lifeTime = 0.0f;
		}
	}
};
/// <summary>
/// �Փ˂Ŕ����f�R���[�^�[
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
				//TODO ��������
			}
		}
		if (!p.m_isCCollisionObj) {
			//���肪CCollisionObj����Ȃ��Ă���
			m_bullet->m_lifeTime = 0.0f;
			//TODO ��������
		}
	}
};

//TODO �n�`����f�R���[�^�[
//TODO ����(MHW) �n�`�m�C�Y
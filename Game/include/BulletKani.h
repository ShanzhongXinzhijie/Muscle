#pragma once
#include"BeamModel.h"

class BulletGO;

/// <summary>
/// �o���b�g��
/// �z�[�~���O�E����O��
/// �ڐG�����E����
/// �����̕ӂ��f�R�邽�߂̂��
/// </summary>
class IBulletDecolator {
public:
	IBulletDecolator(IBulletDecolator* ptrDecolator) :m_decolator(ptrDecolator) {}

	void DecoUpdate() {
		m_decolator->DecoUpdate();
		InnerUpdate();
	}
	void DecoPostUpdate() {
		m_decolator->DecoPostUpdate();
		InnerPostUpdate();
	}
	void DecoContact() {
		m_decolator->DecoContact();
		InnerContact();
	}

	virtual void InnerUpdate() = 0;
	virtual void InnerPostUpdate() = 0;
	virtual void InnerContact() = 0;

	void SetBullet(BulletGO* bullet) {
		m_bullet = bullet;
		if (m_decolator) {
			m_decolator->SetBullet(m_bullet);
		}
	}
	BulletGO* GetBullet() {
		//if (m_decolator) {
		//	return m_decolator->GetBullet();
		//}
		//else {
			return m_bullet;
		//}
	}

private:
	IBulletDecolator* m_decolator = nullptr;
	BulletGO* m_bullet = nullptr;
};

/// <summary>
/// �o���b�g�𓮂����N���X
/// </summary>
class BulletGO : public IQSGameObject{
public:
	BulletGO(IBulletDecolator* ptrDecolator, const CVector3& pos, const CVector3& move);

	void Update()override;
	void PostLoopUpdate()override;

private:
	//�f�R���[�^�[
	IBulletDecolator* m_decolator = nullptr;

	//���f��
	BeamModel m_model;
	bool m_isMoved = false;
	//����
	float m_lifeTime = 3.0f;
	//���W�Ƃ�
	CVector3 m_pos, m_posOld, m_vector;
	//�R���W����
	DHCollision m_col;
};


//TODO �n�`����f�R���[�^�[
//TODO ����(MHW) �n�`�m�C�Y
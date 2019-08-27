#pragma once
#include"BeamModel.h"

class BulletKani :
	public IGameObject
{
public:
	BulletKani(const CVector3& pos, const CVector3& move);

	void Update()override;

private:
	BeamModel m_model;
	float m_lifeTime = 3.0f;
	CVector3 m_pos, m_posOld, m_vector;

	//�R���W����
	DHCollision m_col;
};

/// <summary>
/// �o���b�g��
/// �z�[�~���O�E����O��
/// �ڐG�����E����
/// �����̕ӂ��f�R�邽�߂̂��
/// </summary>
class IBulletDecolator {
public:
	void Update() {
		m_ptrBullet->Update();
		InnerUpdate();
	}
	void Contact() {
		m_ptrBullet->Contact();
		InnerContact();
	}

	virtual void InnerUpdate() = 0;
	virtual void InnerContact() = 0;

private:
	IBulletDecolator* m_ptrBullet = nullptr;
};

/// <summary>
/// �o���b�g�N���X
/// </summary>
class IBullet : public IGameObject {
public:

private:
	//IBulletModel*;
	IBulletDecolator* m_decolator = nullptr;

	float m_lifeTime = 3.0f;
	CVector3 m_pos, m_posOld, m_vector;
};

//TODO ����(MHW) �n�`�m�C�Y
#pragma once

#include<bitset>

//�R���W�����̑���
enum enCollisionAttributes {
	enPhysical,
	enFlame,
	enWind,
	enWater,

	enAttributesNum,
};

/// <summary>
/// �����蔻��ŎQ�Ƃ���\����
/// </summary>
struct ReferenceCollision : public IDW_Class {
public:	
	std::bitset<enAttributesNum> attributes;//�����r�b�g�}�X�N
	CVector3 position;//�ʒu
	CVector3 direction;//����

	float damege = 0.0f;//�_���[�W
};

/// <summary>
/// ���̃Q�[���̓����蔻��I�u�W�F�N�g
/// </summary>
class DHCollision
{
public:
	//�R���X�g���N�^
	DHCollision() {
		//�Փˎ��ɎQ�Ƃ���\���̂�ݒ�
		m_collision.SetName(L"ReferenceCollision");
		m_collision.SetClass(&m_reference);
	}

	//���W��ݒ�
	void SetPos(const CVector3& pos) {
		m_collision.SetPosition(pos);
		m_reference.position = pos;
	}
	//��]��ݒ�
	void SetRot(const CQuaternion& rot) {
		m_collision.SetRotation(rot);
	}

	//������ݒ�
	void SetDir(const CVector3& dir) {
		m_reference.direction = dir;
	}

	//�L�������̐ݒ�
	void SetEnable(bool isEnable) {
		m_collision.SetEnable(isEnable);
	}
	
	//�R���W�����I�u�W�F�N�g
	SuicideObj::CCollisionObj m_collision;
	//�R���W�������
	ReferenceCollision m_reference;
};


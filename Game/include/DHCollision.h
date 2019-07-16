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
};

/// <summary>
/// ���̃Q�[���̓����蔻��I�u�W�F�N�g
/// </summary>
class DHCollision
{
public:
	DHCollision() {
		//�Փˎ��ɎQ�Ƃ���\���̂�ݒ�
		m_collision.SetName(L"ReferenceCollision");
		m_collision.SetClass(&m_reference);
	}

	void SetPos(const CVector3& pos) {
		m_collision.SetPosition(pos);
		m_reference.position = pos;
	}

	SuicideObj::CCollisionObj m_collision;
	ReferenceCollision m_reference;
};


#pragma once

/// <summary>
/// �����蔻��ŎQ�Ƃ���\����
/// </summary>
class ReferenceCollision : public IDW_Class {
public:
	enum enAttributes {
		enPhysical,
		enFlame,
		enWind,
		enWater,

		enAttributesNum,
	};
	unsigned int attributes = 0;//�����r�b�g�}�X�N
	CVector3 position;
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

	SuicideObj::CCollisionObj m_collision;
	ReferenceCollision m_reference;
};


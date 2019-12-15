#pragma once

#include<bitset>

//�R���W�����̃O���[�v
enum enCollisionGroups {
	enDefault,
	enField,//�n�`

	enGroupsNum,
};

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
	CVector3 velocity;//�x���V�e�B
	CVector3 direction;//����

	float damege = 0.0f;//�_���[�W
	float stunTimeSec = 0.0f;//�X�^���b��

	std::function<void(ReferenceCollision*)> m_preCollisionFunc;
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
	void SetVelocity(const CVector3& vec) {
		m_reference.velocity = vec;
		m_reference.direction = vec; m_reference.direction.Normalize();
	}

	//����O���[�v��ݒ�
	void On_OneGroup(unsigned int oneGroup) {
		m_collision.On_OneGroup(oneGroup);
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

namespace DHUtil {
	struct RamReturn {
		RamReturn() = default;
		RamReturn(float p_damege, float p_stunSec) : damege(p_damege), stunSec(p_stunSec) {};
		float damege = 0.0f;
		float stunSec = 0.0f;
	};
	/// <summary>
	/// �̓�����_���[�W���v�Z
	/// </summary>
	/// <param name="mineVelocity">�_���[�W��^���鑤�̃x���V�e�B</param>
	/// <param name="enemyVelocity">���炤���̃x���V�e�B</param>
	/// <returns>�_���[�W�ƃX�^���l</returns>
	static inline RamReturn CalcRamDamege(const CVector3& mineVelocity, const CVector3& enemyVelocity) {
		float damege = max(0.0f, mineVelocity.GetNorm().Dot(mineVelocity - enemyVelocity));
		return damege > 10.0f ? RamReturn(damege, 1.0f) : RamReturn(0.0f, 0.0f);
	}
}
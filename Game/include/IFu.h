#pragma once

/// <summary>
/// �Q�[���t�B�[���h��ɑ��݂�����̂�\���C���^�[�t�F�[�X�N���X
/// </summary>
class IFu 
{
public:
	IFu();

protected:
	//�ʒu��ݒ�
	void SetPos(const CVector3& pos) { m_pos = pos; m_col.SetPos(pos); }
	//��]��ݒ�
	void SetRot(const CQuaternion& rot) { m_rot = rot; m_col.SetRot(rot); }

	/// <summary>
	/// DHCollision�Ƃ̏Փˎ��Ɏ��s����֐�
	/// </summary>
	void SetCollisionFunc(std::function<void(ReferenceCollision*)> func) {
		m_collisionFunc = func;
	}

private:
	CVector3 m_pos;
	CQuaternion m_rot;
	//DW_GETSETCONSREF(CVector3, m_scale, Scale)
	
	DHCollision m_col;
	std::function<void(ReferenceCollision*)> m_collisionFunc;
};


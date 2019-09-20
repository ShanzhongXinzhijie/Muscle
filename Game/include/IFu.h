#pragma once

/// <summary>
/// �Q�[���t�B�[���h��ɑ��݂�����̂�\���C���^�[�t�F�[�X�N���X
/// </summary>
class IFu 
{
public:
	IFu();

	//�ʒu��ݒ�
	void SetPos(const CVector3& pos) { m_pos = pos; UpdateCollisionPos(); }
	//��]��ݒ�
	void SetRot(const CQuaternion& rot) {
		m_rot = rot; m_col.SetRot(m_colRot * m_rot);
		m_front = CVector3::Front(), m_up = CVector3::Up(), m_left = CVector3::Left();
		m_rot.Multiply(m_front);
		m_rot.Multiply(m_up);
		m_rot.Multiply(m_left);
		m_back = m_front * -1.0f, m_down = m_up * -1.0f, m_right = m_left * -1.0f;
	}

	//�ړ�
	void Move(const CVector3& move) { m_pos += move; UpdateCollisionPos(); }

	//�ʒu���擾
	const CVector3& GetPos()const { return m_pos; }
	//��]���擾
	const CQuaternion& GetRot()const { return m_rot; }

	//�O�������擾
	const CVector3& GetFront()const { return m_front; }
	//��������擾
	const CVector3& GetBack()const { return m_back; }
	//��������擾
	const CVector3& GetUp()const { return m_up; }
	//���������擾
	const CVector3& GetDown()const { return m_down; }
	//���������擾
	const CVector3& GetLeft()const { return m_left; }
	//�E�������擾
	const CVector3& GetRight()const { return m_right; }

protected:	
	//�ʒu���擾
	//CVector3& GetPosRef() { return m_pos; }
	//��]���擾
	//CQuaternion& GetRotRef() { return m_rot; }

	/// <summary>
	/// DHCollision�Ƃ̏Փˎ��Ɏ��s����֐�
	/// </summary>
	void SetCollisionFunc(std::function<void(ReferenceCollision*)> func) {
		m_collisionFunc = func;
	}
	//���`�̋�炢������쐬
	void CreateSphere(const CVector3& pos, const CQuaternion& rot, float radius) {
		m_colPos = pos; m_colRot = rot;
		m_col.m_collision.CreateSphere(m_pos + m_colPos, m_colRot * m_rot, radius);
	}
	//�J�v�Z���`�̋�炢������쐬
	void CreateCapsule(const CVector3& pos, const CQuaternion& rot, float radius, float height) {
		m_colPos = pos; m_colRot = rot;
		m_col.m_collision.CreateCapsule(m_pos + m_colPos, m_colRot * m_rot, radius, height);
	}
	//��炢����̈ʒu�I�t�Z�b�g��ݒ�
	void SetCollisionPos(const CVector3& pos) {
		m_colPos = pos;
		UpdateCollisionPos();
	}

private:
	//��炢����̈ʒu���X�V
	void UpdateCollisionPos() {
		CVector3 offset = m_colPos;
		m_rot.Multiply(offset);
		m_col.SetPos(m_pos + offset);
	}

private:
	CVector3 m_pos;
	CQuaternion m_rot;
	//DW_GETSETCONSREF(CVector3, m_scale, Scale)

	CVector3 m_front = CVector3::Front(), m_up = CVector3::Up(), m_left = CVector3::Left();
	CVector3 m_back = CVector3::Back(), m_down = CVector3::Down(), m_right = CVector3::Right();
	
	DHCollision m_col;
	CVector3 m_colPos;
	CQuaternion m_colRot;
	std::function<void(ReferenceCollision*)> m_collisionFunc;
};


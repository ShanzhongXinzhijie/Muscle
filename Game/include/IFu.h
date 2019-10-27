#pragma once

/// <summary>
/// �Q�[���t�B�[���h��ɑ��݂�����̂�\���C���^�[�t�F�[�X�N���X
/// </summary>
class IFu 
{
public:
	//�R���X�g���N�^
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
	//�R���W�����ʒu���擾
	CVector3 GetCollisionPos()const { return m_pos + m_colPos; }
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

//protected:	
	/// <summary>
	/// DHCollision�Ƃ̏Փˎ��Ɏ��s����֐�
	/// </summary>
	void SetCollisionFunc(std::function<void(ReferenceCollision*, SuicideObj::CCollisionObj::SCallbackParam&)> func) {
		m_collisionFunc = func;
	}

	//���̋�炢�����ÓI�I�u�W�F�N�g�Ƃ��Ă����������邩�ݒ�
	//�������蔻��`����쐬����O�ɐݒ肵�Ă�������
	void SetIsStaticObject(bool isStatic) {
		m_col.m_collision.SetIsStaticObject(isStatic);
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
	//���b�V���`�̋�炢������쐬
	void CreateMesh(const GameObj::CSkinModelRender& skinModelRender) {
		m_colPos = skinModelRender.GetPos(); m_colRot = skinModelRender.GetRot();
		m_col.m_collision.CreateMesh(skinModelRender);
	}

	//��炢����̑����r�b�g�}�X�N���擾
	std::bitset<enAttributesNum>& GetAttributes() {
		return m_col.m_reference.attributes;
	}

	//��炢����̔���O���[�v��ݒ�
	void On_OneGroup(unsigned int oneGroup) {
		m_col.On_OneGroup(oneGroup);
	}

	//��炢����̈ʒu�I�t�Z�b�g��ݒ�
	void SetCollisionPos(const CVector3& pos) {
		m_colPos = pos;
		UpdateCollisionPos();
	}

	//��炢����̗L��������ݒ�
	void SetCollisionEnable(bool isEnable) {
		m_col.SetEnable(isEnable);
	}

private:
	//��炢����̈ʒu���X�V
	void UpdateCollisionPos() {
		CVector3 offset = m_colPos;
		m_rot.Multiply(offset);
		m_col.SetPos(m_pos + offset);
	}

private:
	//�ʒu�Ɖ�]
	CVector3 m_pos;
	CQuaternion m_rot;

	//�e����
	CVector3 m_front = CVector3::Front(), m_up = CVector3::Up(), m_left = CVector3::Left();
	CVector3 m_back = CVector3::Back(), m_down = CVector3::Down(), m_right = CVector3::Right();
	
	//�R���W����
	DHCollision m_col;
	CVector3 m_colPos;
	CQuaternion m_colRot;
	std::function<void(ReferenceCollision*, SuicideObj::CCollisionObj::SCallbackParam&)> m_collisionFunc;
};

/// <summary>
/// �����̃I�u�W�F�N�g
/// </summary>
class ILifeObject : public IGameObject, public IFu {
public:
	ILifeObject() {
		SetName(L"LockableObject");
	}

	//����
	void Death() {
		if (m_deathFunc) { m_deathFunc(); }
		SetEnable(false);//�Q�[���I�u�W�F�N�g������
		SetCollisionEnable(false);//�����蔻�薳��
		if (m_isAutoDelete) { delete this; }//���E
	}

	//���S���ɃC���X�^���X���폜���邩�ݒ�
	void SetIsAutoDelete(bool isDelete) {
		m_isAutoDelete = isDelete;
	}

	//���S���Ɏ��s����֐���ݒ�
	void SetDeathFunc(std::function<void()> func) {
		m_deathFunc = func;
	}

private:
	bool m_isAutoDelete = true;
	std::function<void()> m_deathFunc;
};
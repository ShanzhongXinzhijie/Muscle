#pragma once

/// <summary>
/// �Q�[���t�B�[���h��ɑ��݂�����̂�\���C���^�[�t�F�[�X�N���X
/// </summary>
class IFu 
{
public:
	//�R���X�g���N�^
	IFu();
	//�f�X�g���N�^
	virtual ~IFu() {
		//���S�t���O�𗧂Ăč폜��ʒm����
		for (auto& flag : m_deleteFlagList) {
			if (!flag.expired()) {//�폜����ĂȂ����
				*flag.lock() = true;
			}
		}
	}

	//�ʒm�p���S�t���O��ݒ�
	void SetDeleteFlag(std::weak_ptr<bool> flagPtr) {
		m_deleteFlagList.push_back(flagPtr);
	}

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
	/// <summary>
	/// CollisionFunc(��)�̑O�Ɏ��s����֐�
	/// </summary>
	void SetPreCollisionFunc(std::function<void(ReferenceCollision*)> func) {
		m_col.m_reference.m_preCollisionFunc = func;
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
	//��炢����̍U���͂�ݒ�
	void SetDamegePower(float dame) {
		m_col.m_reference.damege = dame;
	}
	//��炢����̃X�^���b����ݒ�
	void SetStunSec(float sec) {
		m_col.m_reference.stunTimeSec = sec;
	}
	//��炢����̃x���V�e�B�p�����[�^��ݒ�
	void SetCollisionVelocity(const CVector3& velocity) {
		m_col.SetVelocity(velocity);
	}
	//��炢����̃p�����[�^���擾
	const ReferenceCollision& GetReferenceCollision()const {
		return m_col.m_reference;
	}

	//��炢����̔���O���[�v��ݒ�
	void On_OneGroup(unsigned int oneGroup) {
		m_col.On_OneGroup(oneGroup);
	}

	//��炢����̈ʒu�I�t�Z�b�g��ݒ�
	void SetCollisionPosOffset(const CVector3& pos) {
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

	//�ʒm
	std::list<std::weak_ptr<bool>> m_deleteFlagList;
};

//���b�N�ł���I�u�W�F�N�g���b�p�[
class LockableWrapper : public IQSGameObject {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="fu">���b�N�Ώ�</param>
	/// <param name="hpPtr">HP�̃|�C���^</param>
	/// <param name="owner">���b�N�Ώۂ̃I�[�i�[</param>
	LockableWrapper(IFu* fu, float* hpPtr = nullptr, IFu* owner = nullptr)
		: m_fu(fu),m_hpPtr(hpPtr),m_owner(owner)
	{
		SetName(L"LockableObject");
	}

	//HP��ݒ�
	void SetHPRef(float* hpPtr) {
		m_hpPtr = hpPtr;
	}

	//���t���[���̏���
	void PreUpdate()override {
		if (m_isFirstUpdate) {
			m_isFirstUpdate = false;
			m_oldPos = m_fu->GetPos();
			return;
		}
		m_move = m_fu->GetPos() - m_oldPos;
		m_oldPos = m_fu->GetPos();
	}

	//�{�̂��擾
	const IFu* GetFu()const {
		return m_fu;
	}
	IFu* GetFu() {
		return m_fu;
	}

	//�ړ��ʂ��擾
	const CVector3& GetMove()const {
		return m_move;
	}

	//HP���擾
	float GetHP()const {
		if (m_hpPtr) {
			return *m_hpPtr;
		}
		return -1.0f;
	}

	//���b�N�Ώۂ̃I�[�i�[���擾
	const IFu* GetOwner()const {
		return m_owner;
	}

private:
	IFu* m_fu = nullptr;
	IFu* m_owner = nullptr;

	bool m_isFirstUpdate = true;
	CVector3 m_move;//�ړ���(�΍��ˌ��Ɏg��)
	CVector3 m_oldPos;

	float* m_hpPtr = nullptr;
};

/// <summary>
/// �����̃I�u�W�F�N�g
/// </summary>
class ILifeObject : public IGameObject, public IFu {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="isLockable">���b�N�I���\��?</param>
	/// <param name="owner">���̃I�u�W�F�N�g�̃I�[�i�[</param>
	ILifeObject(bool isLockable = true, IFu* owner = nullptr) {
		if (isLockable) {
			m_lockableWrapper = std::make_unique<LockableWrapper>(this, nullptr, owner);
		}
	}

	//���b�N�I���p�N���X�擾
	LockableWrapper* GetLockableWrapper() {
		return m_lockableWrapper ? m_lockableWrapper.get() : nullptr;
	}

	//HP��ݒ�
	void SetHPRef(float* hpPtr) {
		if (m_lockableWrapper) {
			m_lockableWrapper->SetHPRef(hpPtr);
		}
	}

	//���b�N�I���\���ݒ�
	void SetLockable(bool isLockable, IFu* owner) {
		if (isLockable) {
			if (!m_lockableWrapper) {
				m_lockableWrapper = std::make_unique<LockableWrapper>(this, nullptr, owner);
			}
		}
		else {
			m_lockableWrapper.reset();
		}
	}

	//����
	void Death() {
		if (m_deathFunc) { m_deathFunc(); }
		SetEnable(false);//�Q�[���I�u�W�F�N�g������
		SetCollisionEnable(false);//�����蔻�薳��
		if (m_isAutoDelete) { delete this; return; }//���E
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
	std::unique_ptr<LockableWrapper> m_lockableWrapper;
};
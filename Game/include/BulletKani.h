#pragma once
#include"IFu.h"

class BulletGO;

/// <summary>
/// �o���b�g�R���|�[�l���g
/// </summary>
class IBulletComponent {
public:
	virtual ~IBulletComponent() {}

	virtual void Start() {}
	virtual void PreLoopUpdate() {}
	virtual void Update() {}
	virtual void PostUpdate()  {}
	virtual void PostLoopUpdate() {}
	virtual void Pre3DRender(int) {}
	virtual void Contact(SuicideObj::CCollisionObj::SCallbackParam& p) {}
	virtual bool PreContact(ReferenceCollision* refcol) { return true; }

	//�ݒ茳��ݒ�
	void SetBullet(BulletGO* bullet) {
		m_bullet = bullet;
	}

	//�R���|�[�l���g���L�����ݒ�
	void SetEnable(bool b) {
		m_enable = b;
	}
	bool GetEnable()const {
		return m_enable;
	}

protected:
	bool m_enable = true;
	BulletGO* m_bullet = nullptr;
};

/// <summary>
/// �o���b�g�𓮂����N���X
/// </summary>
class BulletGO : public ILifeObject, public ICreateObject {
public:
	BulletGO(const CVector3& pos, const CVector3& move, IFu* owner, float damege, bool isLockable = false, int priorityLevel = LockableWrapper::DEFAULT_LEVEL);

	void PreLoopUpdate()override;
	void Update()override;
	void PostLoopUpdate()override;
	void Pre3DRender(int)override;

	/// <summary>
	/// �X�^�����Ԃ�ݒ�
	/// </summary>
	void SetStunTimeSec(float stunTimeSec) {
		m_col.m_reference.stunTimeSec = stunTimeSec;
	}

	/// <summary>
	/// �R���|�[�l���g��ǉ�����
	/// </summary>
	void AddComponent(std::unique_ptr<IBulletComponent>&& component) {
		m_components.emplace_back(std::move(component));
		m_components.back()->SetBullet(this);
		m_components.back()->Start();
	}
	/// <summary>
	/// �R���|�[�l���g���擾
	/// </summary>
	IBulletComponent& GetComponent(int i) {
		return *m_components[i].get();
	}
	/// <summary>
	/// �Ō�ɒǉ������R���|�[�l���g���擾
	/// </summary>
	IBulletComponent& GetComponentBack() {
		return *m_components.back().get();
	}	

	/// <summary>
	/// �I�[�i�[�̎擾
	/// </summary>
	IFu* GetOwner() {
		return m_owner;
	}

	/// <summary>
	/// �����W�̎擾
	/// </summary>
	const CVector3& GetOldPos()const { return m_posOld; }

	/// <summary>
	/// ���t���[���̃x���V�e�B�̕ω����v�Z
	/// </summary>
	static void CalcVelocityUpdate(CVector3& velocity, float gravity = 0.0f, float upBrake = 0.025f, float downAccel = 0.25f);

private:
	//���̒e�̃I�[�i�[
	IFu* m_owner = nullptr;
	//�R���|�[�l���g
	std::vector<std::unique_ptr<IBulletComponent>> m_components;	
	//�Í��W
	CVector3 m_posOld;
	//�������W
	CVector3 m_defaultPos;
	//�����U����
	float m_defaultDamege = 0.0f;
	//�_���[�W�u�[�X�g�G�t�F�N�g
	std::unique_ptr<CBillboard> m_damegeBoostEffect;
public:
	//����
	float m_lifeTime = 3.0f*FRAME_RATE;
	//�i�s�x�N�g��
	CVector3 m_vector;
	//�U���R���W����
	DHCollision m_col;
	//���a
	float m_radius = 0.0f;
	//�d��
	float m_gravity = 0.0f;
	//�㏸����
	float m_upBrake = 0.025f;
	//���~����
	float m_downAccel = 0.25f;
	//�����ɂ��_���[�W�u�[�X�g�̗L���t���O
	bool m_isDamegeHeightBoost = true;
};

/// <summary>
/// �^�C�}�[�R���|�[�l���g
/// ��莞�Ԍ�ɃR���|�[�l���g��L�����E����������
/// </summary>
/// <param name="timer">�ݒ莞�ԁB���s�܂ł̃t���[����</param>
/// <param name="willEnable">true�Őݒ莞�Ԍ�R���|�[�l���g��L�����Bfalse�Ȃ疳����</param>
class BD_Timer : public IBulletComponent {
public:
	BD_Timer(float timer = 0.0f, bool willEnable = true)
		: m_timerf(timer), m_willEnable(willEnable)
	{};

	void Update()override {
		if (m_timerf < FLT_EPSILON) { return; }
		//�^�C�}�[����
		m_timerf = max(0.0f, m_timerf - 1.0f);
		if (m_timerf < FLT_EPSILON) {
			for (auto& com : m_components) {
				com->SetEnable(m_willEnable);
			}
		}
	}

	/// <summary>
	/// �R���|�[�l���g��ǉ�
	/// </summary>
	void AddComponent(IBulletComponent* comp) {
		if (comp) {
			comp->SetEnable(!m_willEnable);//�L���E�����̐ݒ�ύX
			m_components.emplace_back(comp);
		}
	}

private:
	float m_timerf = 0.0f;
	bool m_willEnable = true;
	std::list<IBulletComponent*> m_components;
};

/// <summary>
/// ���b�N�I���\���R���|�[�l���g
/// </summary>
class BD_Lockable :public IBulletComponent {
public:
	BD_Lockable(int lockLevel = LockableWrapper::DEFAULT_LEVEL) :m_lockLevel(lockLevel) {}

	void Start()override {
		m_bullet->SetLockable(true, m_bullet->GetOwner());
	}

private:
	int m_lockLevel = LockableWrapper::DEFAULT_LEVEL;
};
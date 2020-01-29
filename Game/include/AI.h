#pragma once

class CDeathHotoke;

/// <summary>
/// AI�̐��ݏo���X�e�[�^�X
/// </summary>
struct AIStatus {
	bool isMovingToTarget = false;//�ڕW�Ɉړ����邩?
	CVector3 moveTargetPosition;//�ړ��������ʒu
	bool isAttackingTarget = false;//�ڕW���U�����邩?
};

/// <summary>
/// AI�̃C���^�[�t�F�[�X
/// </summary>
class IAI
{
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="ptrCore">�{�̂̃|�C���^</param>
	IAI(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore){};
	IAI() = default;

	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	virtual ~IAI() {};

	/// <summary>
	/// �������쐬
	/// </summary>
	virtual IAI* Create(CDeathHotoke* ptrCore = nullptr)const = 0;

	/// <summary>
	/// ���t���[����Ԃ��
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �X�e�[�^�X�o��
	/// </summary>
	const AIStatus& GetOutputStatus()const { return m_outputStatus; }

protected:
	CDeathHotoke* m_ptrCore = nullptr;
	AIStatus m_outputStatus;
};

/// <summary>
/// �e�X�gAI
/// </summary>
class TestAI : public IAI {
public:
	using IAI::IAI;

	IAI* Create(CDeathHotoke* ptrCore = nullptr)const override {
		return new TestAI(ptrCore ? ptrCore : m_ptrCore);
	}

	void Update()override;
};
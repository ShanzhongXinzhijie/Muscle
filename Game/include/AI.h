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
	IAI(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore){};
	virtual ~IAI() {};

	virtual void Update() = 0;

	/// <summary>
	/// �X�e�[�^�X�o��
	/// </summary>
	const AIStatus& GetOutputStatus()const { return m_outputStatus; }

protected:
	CDeathHotoke* m_ptrCore = nullptr;
	AIStatus m_outputStatus;
};

class TestAI : public IAI {
public:
	using IAI::IAI;
	void Update()override;
};
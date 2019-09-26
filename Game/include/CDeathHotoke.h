#pragma once
#include"IGamePad.h"
#include"AI.h"
#include"BodyPartsHeader.h"
#include"IFu.h"

class CDeathHotoke :
	public ILifeObject
{
public:
	//�R���X�g���N�^
	CDeathHotoke(IGamePad* ptrPad, bool isDrawHUD, std::unique_ptr<IAI> AI):m_ptrPad(ptrPad),m_isDrawHUD(isDrawHUD),m_ai(std::move(AI)){
		SetName(L"CDeathHotoke");
	}

	//�{�f�B�p�[�c�̎��
	enum enBodyParts {
		enWing, enLeg, enArm, enHead, enPartsNum,
	};
	//�p�[�c�ݒ�
	void SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part);

	//IGameObject�֌W
	bool Start()override;
	void PreUpdate()override;
	void Update()override;
	void PostLoopUpdate()override;
	void PostRender()override;

	//�ړ��ʂ�������
	void AddMove(const CVector3& vec) { m_move += vec; }
	void AddRot(const CQuaternion& rot) { m_rotMove = rot * m_rotMove; }

	//�_���[�W����������
	void Damage(const ReferenceCollision& ref, const CVector3& pos);

	//�^�[�Q�b�g�ʒu��ݒ�
	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	
	//�Q�b�^�[
	[[nodiscard]] const CVector3& GetScale()const { return m_scale; }
	[[nodiscard]] CVector3 GetMove() const { return GetPos() - m_posOld; }
	//�ړ��ʂ��擾
	[[nodiscard]] const CVector3& GetVelocity() const { return m_move; }
	//�^�[�Q�b�g�ʒu���擾
	[[nodiscard]] const CVector3& GetTargetPos() const { return m_targetPos; }
	
	//�p�b�h�̎擾
	[[nodiscard]] IGamePad* GetPad() { return m_ptrPad; }

	//HUD��\�����邩�擾
	[[nodiscard]] bool GetIsDrawHUD()const { return m_isDrawHUD; }

	//AI�̐��ݏo���X�e�[�^�X���擾
	[[nodiscard]] const AIStatus* GetAIStatus()const { if (m_ai) { return &m_ai->GetOutputStatus(); } return nullptr; }

private:
	//�R�A�̃��f��
	GameObj::CSkinModelRender m_coreModel;
	
	//�X�P�[��
	CVector3 m_scale;

	//�O�t���[���̈ʒu
	CVector3 m_posOld;
	
	//�ړ���
	CVector3 m_move;
	CQuaternion m_rotMove;

	//�X�e�[�^�X
	float m_hp = 100.0f;

	//�p�[�c
	std::unique_ptr<IBodyPart> m_parts[4];

	//�Q�[���p�b�h
	IGamePad* m_ptrPad = nullptr;

	//HUD��\�����邩
	bool m_isDrawHUD = false;

	//�^�[�Q�b�g�ʒu
	CVector3 m_targetPos;

	//AI
	std::unique_ptr<IAI> m_ai;
};


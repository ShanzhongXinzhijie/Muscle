#pragma once
#include"IGamePad.h"
#include"AI.h"

class IBodyPart;

class CDeathHotoke :
	public IGameObject
{
public:
	//�R���X�g���N�^
	CDeathHotoke(IGamePad* ptrPad, bool isDrawHUD, std::unique_ptr<IAI> AI):m_ptrPad(ptrPad),m_isDrawHUD(isDrawHUD),m_ai(std::move(AI)){
		SetName(L"CDeathHotoke");
	}

	//IGameObject�֌W
	bool Start()override;
	void Update()override;
	void PostRender()override;

	//�ړ��ʂ�������
	void AddMove(const CVector3& vec) { m_move += vec; }
	void AddRot(const CQuaternion& rot) { m_rotMove = rot * m_rotMove; }

	//�_���[�W����������
	void Damage(const ReferenceCollision& ref);

	//�Z�b�^�[
	void SetPos(const CVector3& pos) { m_pos = pos; }
	void SetRot(const CQuaternion& rot) { m_rot = rot; }
	//�^�[�Q�b�g�ʒu��ݒ�
	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	
	//�Q�b�^�[
	[[nodiscard]] const CVector3& GetPos()const { return m_pos; }
	[[nodiscard]] const CQuaternion& GetRot()const { return m_rot; }
	[[nodiscard]] const CVector3& GetScale()const { return m_scale; }
	//�ړ��ʂ��擾
	[[nodiscard]] const CVector3& GetMove() const { return m_move; }
	//�^�[�Q�b�g�ʒu���擾
	[[nodiscard]] const CVector3& GetTargetPos() const { return m_targetPos; }
	
	//�O�x�N�g�����擾
	[[nodiscard]] const CVector3& GetFront()const { return m_front; }
	//��x�N�g�����擾
	[[nodiscard]] const CVector3& GetBack()const { return m_back; }
	//���x�N�g�����擾
	[[nodiscard]] const CVector3& GetLeft()const { return m_left; }
	//�E�x�N�g�����擾
	[[nodiscard]] const CVector3& GetRight()const { return m_right; }
	//��x�N�g�����擾
	[[nodiscard]] const CVector3& GetUp()const { return m_up; }
	//���x�N�g�����擾
	[[nodiscard]] const CVector3& GetDown()const { return m_down; }

	//�p�b�h�̎擾
	[[nodiscard]] IGamePad* GetPad() { return m_ptrPad; }

	//HUD��\�����邩�擾
	[[nodiscard]] bool GetIsDrawHUD()const { return m_isDrawHUD; }

	//AI�̐��ݏo���X�e�[�^�X���擾
	[[nodiscard]] const AIStatus* GetAIStatus()const { if (m_ai) { return &m_ai->GetOutputStatus(); } return nullptr; }

private:
	//�����x�N�g�����v�Z�E�X�V
	void CalcDirection();

private:
	//�R�A�̃��f��
	GameObj::CSkinModelRender m_coreModel;

	//�R���W����
	DHCollision m_col;

	//TRS
	CVector3 m_pos, m_scale;
	CQuaternion m_rot;

	//�����x�N�g��
	CVector3 m_front = CVector3::Front(), m_left = CVector3::Left(), m_up = CVector3::Up();
	CVector3 m_back = CVector3::Back(), m_right = CVector3::Right(), m_down = CVector3::Down();

	//�ړ���
	CVector3 m_move;
	CQuaternion m_rotMove;

	//�X�e�[�^�X
	float m_hp = 100.0f;

	//�p�[�c
	IBodyPart* m_parts[4] = {};

	//�Q�[���p�b�h
	IGamePad* m_ptrPad = nullptr;

	//HUD��\�����邩
	bool m_isDrawHUD = false;

	//�^�[�Q�b�g�ʒu
	CVector3 m_targetPos;

	//AI
	std::unique_ptr<IAI> m_ai;

public:
	//�{�f�B�p�[�c�̎��
	enum enBodyParts {
		enWing, enLeg, enArm, enHead, enPartsNum, 
	};
};


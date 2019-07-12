#pragma once
#include"IGamePad.h"

class IBodyPart;

class CDeathHotoke :
	public IGameObject
{
public:
	CDeathHotoke(IGamePad* ptrPad):m_ptrPad(ptrPad){}

	bool Start()override;
	void Update()override;
	void PostRender()override;

	//�ړ��ʂ�������
	void AddMove(const CVector3& vec) { m_move += vec; }
	void AddRot(const CQuaternion& rot) { m_rotMove = rot * m_rotMove; }

	//�Z�b�^�[
	void SetPos(const CVector3& pos) { m_pos = pos; }
	void SetRot(const CQuaternion& rot) { m_rot = rot; }

	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	
	//�Q�b�^�[
	const CVector3& GetPos()const { return m_pos; }	
	const CQuaternion& GetRot()const { return m_rot; }
	const CVector3& GetScale()const { return m_scale; }

	const CVector3& GetTargetPos() { return m_targetPos; }

	//�p�b�h�̎擾
	IGamePad* GetPad() { return m_ptrPad; }

private:
	//�R�A�̃��f��
	GameObj::CSkinModelRender m_coreModel;

	//TRS
	CVector3 m_pos, m_scale;
	CQuaternion m_rot;

	//�ړ���
	CVector3 m_move;
	CQuaternion m_rotMove;

	//�p�[�c
	IBodyPart* m_parts[4] = {};

	//�Q�[���p�b�h
	IGamePad* m_ptrPad = nullptr;

	//�^�[�Q�b�g�ʒu
	CVector3 m_targetPos;

public:
	//�{�f�B�p�[�c�̎��
	enum enBodyParts {
		enWing, enLeg, enArm, enHead, enPartsNum, 
	};
};


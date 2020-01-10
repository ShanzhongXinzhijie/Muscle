#pragma once

#include"LOD.h"
#include"StageObjectGenerator.h"

/// <summary>
/// ��
/// </summary>
//class Stone : public IStageObject {
//public:
//	using IStageObject::IStageObject;
//
//	//�������֐�
//	void Init(const CVector3& pos, const CVector3& normal)override;
//
//private:
//	//�O���t�B�b�N
//	GameObj::CInstancingModelRender m_model;
//
//public:
//	static inline int m_sInstancingMax = 512; //���̃N���X�̍ő�C���X�^���X��
//};

/// <summary>
/// �S��
/// </summary>
class TransmissionTower : public IStageObject, public IGameObject {
public:	
	using IStageObject::IStageObject;

	//�������֐�
	void Init(const CVector3& pos, const CVector3& normal)override;

	//���f�����擾
	GameObj::CSkinModelRender& GetModel() {
		return m_model;
	}

	//���C���[��ʂ���Ă��邩?
	void SetIsWired(bool b) { m_isWired = b; }
	bool IsWired()const { return m_isWired; }

private:
	//�O���t�B�b�N
	GameObj::CSkinModelRender m_model;
	std::unique_ptr<GameObj::CSkinModelRender> m_wire[6];
	//std::unique_ptr<GameObj::CSkinModelRender> m_dodai[4];

	bool m_isWired = false;//���C���[�ʂ���Ă邩?
};

/// <summary>
/// ��
/// </summary>
class Grass : public IGameObject {
public:
	//�J�n������
	bool Start()override;
	//���샋�[�v�㏈��
	void PostLoopUpdate()override;
	//�`��O����
	void Pre3DRender(int)override;

	//�ǂ̃J�����ɕ\�����邩�ݒ�
	void SetDrawCameraNum(int camnum) {
		m_cameraNum = camnum;
	}

private:
	//�ʒu���Đݒ�
	void RePos(GameObj::ICamera* mainCamera);

private:
	//�O���t�B�b�N
	GameObj::CInstancingModelRender m_model;

	int m_cameraNum = 0;//�ǂ̃J�����ɕ\�����邩

public:
	static inline constexpr int m_sInstancingMax = 512 * 4 * PLAYER_NUM; //���̃N���X�̍ő�C���X�^���X��
};

/// <summary>
/// �؂Ł[��
/// </summary>
class Tree : public IStageObject{
public:
	using IStageObject::IStageObject;

	//�������֐�
	void Init(const CVector3& pos, const CVector3& normal)override;

private:
	//�O���t�B�b�N
	LODSwitcher m_lodSwitcher;
	LODInstancingModel m_model;
	LODImposter m_imposter;

	//���W�Ƃ�
	CVector3 m_pos;
	CQuaternion m_rot, m_rotOffset;

	//�����蔻��
	DHCollision m_col;
	bool m_isHited = false;
	float m_modelHeight = 100.0f, m_modelRadius = 0.0f;

public:
	static inline int m_sInstancingMax = 512; //���̃N���X�̍ő�C���X�^���X��
};
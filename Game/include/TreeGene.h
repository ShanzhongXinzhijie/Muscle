#pragma once

#include"LOD.h"
#include"StageObjectGenerator.h"

/// <summary>
/// ��
/// </summary>
class Stone : public IStageObject {
public:
	using IStageObject::IStageObject;

	//�������֐�
	void Init(const CVector3& pos, const CVector3& normal)override;

private:
	//�O���t�B�b�N
	GameObj::CInstancingModelRender m_model;

public:
	static int m_sInstancingMax; //���̃N���X�̍ő�C���X�^���X��
};

/// <summary>
/// ��
/// </summary>
class Grass : public IStageObject, public IGameObject {
public:
	using IStageObject::IStageObject;

	//�������֐�
	void Init(const CVector3& pos, const CVector3& normal)override;
	
	//�`��O����
	void Pre3DRender(int)override;

private:
	//�O���t�B�b�N
	//LODSwitcher m_lodSwitcher;
	//LODNothing m_nothing;
	LODInstancingModel m_model;

	int m_cameraNum = 0;//�ǂ̃J�����ɕ\�����邩

public:
	static int m_sInstancingMax; //���̃N���X�̍ő�C���X�^���X��
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
	static int m_sInstancingMax; //���̃N���X�̍ő�C���X�^���X��
};

/*
/// <summary>
/// �؁X�𐶐�����N���X
/// </summary>
class TreeGene
{
public:
	//�؁X�̐���
	void Generate(const CVector3& minArea, const CVector3& maxArea, int num);
	//�؁X��łڂ�
	void Clear();

	//�؂̃��f���擾
	GameObj::CInstancingModelRender& GetTreeModel(int index) {
		return m_trees[index]->GetModel();
	}
	//�؂̐����擾
	size_t GetTreeNum()const { return m_trees.size(); }

private:
	std::vector<std::unique_ptr<Tree>> m_trees;//�؁X�̃��X�g
};
*/

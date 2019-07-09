#pragma once

/// <summary>
/// �؂Ł[��
/// </summary>
class Tree : public IGameObject{
public:
	Tree(int id, const CVector3& pos, const CQuaternion& rot);

	void PostLoopUpdate()override;

	//���f�����擾
	GameObj::CInstancingModelRender& GetModel() { return m_model; }

private:
	int m_id = -1;

	GameObj::CInstancingModelRender m_model;
	CImposter m_imposter;

	CVector3 m_pos;
	CQuaternion m_rot, m_rotOffset;

	SuicideObj::CCollisionObj m_col;
	bool m_isHited = false;

public:
	static int m_sInstancingMax;
};

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
	std::vector<std::unique_ptr<Tree>> m_trees;
};


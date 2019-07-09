#pragma once

/// <summary>
/// 木でーす
/// </summary>
class Tree : public IGameObject{
public:
	Tree(int id, const CVector3& pos, const CQuaternion& rot);

	void PostLoopUpdate()override;

	//モデルを取得
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
	//木々の生成
	void Generate(const CVector3& minArea, const CVector3& maxArea, int num);
	//木々を滅ぼす
	void Clear();

	//木のモデル取得
	GameObj::CInstancingModelRender& GetTreeModel(int index) {
		return m_trees[index]->GetModel();
	}
	//木の数を取得
	size_t GetTreeNum()const { return m_trees.size(); }

private:
	std::vector<std::unique_ptr<Tree>> m_trees;
};


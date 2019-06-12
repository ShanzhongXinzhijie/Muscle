#pragma once

/// <summary>
/// ñÿÇ≈Å[Ç∑
/// </summary>
class Tree : public IGameObject{
public:
	Tree(int id, const CVector3& pos, const CQuaternion& rot);

	void PostLoopUpdate()override;

private:
	int m_id = -1;
	GameObj::CInstancingModelRender m_model;
	CImposter m_imposter;
	//CBillboard m_bill;
	CVector3 m_pos;
	CQuaternion m_rot, m_rotOffset;

	//SuicideObj::CCollisionObj m_col;
	bool m_isHited = false;

public:
	static int m_sInstancingMax;
};

class TreeGene
{
public:
	//ñÿÅXÇÃê∂ê¨
	void Generate(const CVector3& minArea, const CVector3& maxArea, int num);
	//ñÿÅXÇñ≈Ç⁄Ç∑
	void Clear();

private:
	std::vector<std::unique_ptr<Tree>> m_trees;
};


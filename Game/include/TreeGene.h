#pragma once

#include"LOD.h"
#include"StageObjectGenerator.h"

/// <summary>
/// 石
/// </summary>
class Stone : public IStageObject {
public:
	using IStageObject::IStageObject;

	//初期化関数
	void Init(const CVector3& pos, const CVector3& normal)override;

private:
	//グラフィック
	GameObj::CInstancingModelRender m_model;

public:
	static int m_sInstancingMax; //このクラスの最大インスタンス数
};

/// <summary>
/// 木でーす
/// </summary>
class Tree : public IGameObject, public IStageObject{
public:
	using IStageObject::IStageObject;

	//初期化関数
	void Init(const CVector3& pos, const CVector3& normal)override;

	//void PostLoopUpdate()override;

	//モデルを取得
	GameObj::CInstancingModelRender& GetModel() { return m_model.Get(); }

private:
	//グラフィック
	LODSwitcher m_lodSwitcher;
	LODInstancingModel m_model;
	LODImposter m_imposter;

	//座標とか
	CVector3 m_pos;
	CQuaternion m_rot, m_rotOffset;

	//当たり判定
	DHCollision m_col;
	bool m_isHited = false;
	float m_modelHeight = 100.0f, m_modelRadius = 0.0f;

public:
	static int m_sInstancingMax; //このクラスの最大インスタンス数
};

/*
/// <summary>
/// 木々を生成するクラス
/// </summary>
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
	std::vector<std::unique_ptr<Tree>> m_trees;//木々のリスト
};
*/

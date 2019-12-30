#pragma once

#include"LOD.h"
#include"StageObjectGenerator.h"

/// <summary>
/// 石
/// </summary>
//class Stone : public IStageObject {
//public:
//	using IStageObject::IStageObject;
//
//	//初期化関数
//	void Init(const CVector3& pos, const CVector3& normal)override;
//
//private:
//	//グラフィック
//	GameObj::CInstancingModelRender m_model;
//
//public:
//	static inline int m_sInstancingMax = 512; //このクラスの最大インスタンス数
//};

/// <summary>
/// 鉄塔
/// </summary>
class TransmissionTower : public IStageObject {
public:
	using IStageObject::IStageObject;

	//初期化関数
	void Init(const CVector3& pos, const CVector3& normal)override;

private:
	//グラフィック
	GameObj::CSkinModelRender m_model;
	std::unique_ptr<GameObj::CSkinModelRender> m_dodai[4];
};

/// <summary>
/// 草
/// </summary>
class Grass : public IStageObject, public IGameObject {
public:
	using IStageObject::IStageObject;

	//初期化関数
	void Init(const CVector3& pos, const CVector3& normal)override;
	
	//動作ループ後処理
	void PostLoopUpdate()override;
	//描画前処理
	void Pre3DRender(int)override;

private:
	//グラフィック
	//LODSwitcher m_lodSwitcher;
	//LODNothing m_nothing;
	LODInstancingModel m_model;

	int m_cameraNum = 0;//どのカメラに表示するか

public:
	static inline int m_sInstancingMax = 512; //このクラスの最大インスタンス数
};

/// <summary>
/// 木でーす
/// </summary>
class Tree : public IStageObject{
public:
	using IStageObject::IStageObject;

	//初期化関数
	void Init(const CVector3& pos, const CVector3& normal)override;

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
	static inline int m_sInstancingMax = 512; //このクラスの最大インスタンス数
};
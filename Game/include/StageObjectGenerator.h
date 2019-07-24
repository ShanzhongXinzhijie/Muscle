#pragma once

#include"ShadowMapBaker.h"

/// <summary>
/// ステージオブジェクト
/// </summary>
class IStageObject {
public:
	//コンストラクタ
	IStageObject(int id) : m_localID(id), m_worldID(m_s_nextID){
		m_s_nextID++;
	}
	//デストラクタ
	virtual ~IStageObject() {};

	//初期化関数
	virtual void Init(const CVector3& pos, const CVector3& normal) = 0;	

	//シャドウマップベイカーに追加する
	virtual void AddToShadowMapBaker(ShadowMapBaker& targetBaker) {};

private:
	int m_localID = -1, m_worldID = -1;
	static int m_s_nextID;
};

/// <summary>
/// ステージオブジェクトを生成するクラスゥ
/// </summary>
class StageObjectGenerator
{
public:
	//コンストラクタ
	StageObjectGenerator() = default;

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="minArea">生成範囲(最小値)</param>
	/// <param name="maxArea">生成範囲(最大値)</param>
	/// <param name="num">生成するオブジェクトの数</param>
	template <typename T>
	void Generate(const CVector3& minArea, const CVector3& maxArea, int num) {
		//ステージオブジェクトを作る
		for (int i = 0; i < num; i++) {
			//座標生成
			CVector3 pos = { minArea.x + (maxArea.x - minArea.x)*CMath::RandomZeroToOne(), maxArea.y, minArea.z + (maxArea.z - minArea.z)*CMath::RandomZeroToOne() };
			//レイで判定
			btVector3 rayStart = pos;
			btVector3 rayEnd = btVector3(pos.x, minArea.y, pos.z);
			btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
			GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
			if (gnd_ray.hasHit()) {
				//接触点を座標に
				pos = gnd_ray.m_hitPointWorld;
			}
			//生まれろ!
			m_objects.emplace_back(std::make_unique<T>(i));
			m_objects.back()->Init(pos, gnd_ray.m_hitNormalWorld);
		}
	}
	//滅ぼす
	void Clear();

	//ステージオブジェクト取得
	IStageObject& GetModel(int index) { return *m_objects[index]; }

	//数を取得
	size_t GetNum()const { return m_objects.size(); }

private:
	std::vector<std::unique_ptr<IStageObject>> m_objects;//ステージオブジェクト
};


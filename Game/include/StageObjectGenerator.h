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
	/// <param name="maxnum">生成するオブジェクトの最大数</param>
	template <typename T>
	void RectangularGenerate(const CVector3& minArea, const CVector3& maxArea, int maxnum, float radius = 80.0f) {
		int start = (int)m_genPoints.size();
		//生成点作る
		CMath::GenerateBlueNoise(maxnum, { minArea.x,minArea.z }, { maxArea.x,maxArea.z }, radius, m_genPoints);
		//ステージオブジェクトを作る
		for (int i = start; i < m_genPoints.size(); i++) {
			//座標生成
			CVector3 pos = { m_genPoints[i].x, maxArea.y, m_genPoints[i].y };//{ minArea.x + (maxArea.x - minArea.x)*CMath::RandomZeroToOne(), maxArea.y, minArea.z + (maxArea.z - minArea.z)*CMath::RandomZeroToOne() };
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

	/// <summary>
	/// 生成(円)
	/// </summary>
	/// <param name="point">生成中心</param>
	/// <param name="area">生成範囲</param>
	/// <param name="height">生成範囲(高さ)</param>
	/// <param name="maxnum">生成するオブジェクトの最大数</param>
	template <typename T>
	void CircularGenerate(const CVector3& point, float area, float height, int maxnum, float radius = 80.0f) {
		int start = (int)m_genPoints.size();
		//生成点作る
		CMath::GenerateBlueNoise(maxnum, { point.x - area, point.z - area }, { point.x + area, point.z + area }, radius, m_genPoints);
		//ステージオブジェクトを作る
		for (int i = start; i < m_genPoints.size(); i++) {
			//生成点は円内か?
			if ((CVector2(point.x, point.z) - m_genPoints[i]).LengthSq() < CMath::Square(area)) {
				//座標生成
				CVector3 pos = { m_genPoints[i].x, point.y + height, m_genPoints[i].y };
				//レイで判定
				btVector3 rayStart = pos;
				btVector3 rayEnd = btVector3(pos.x, point.y - height, pos.z);
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
	}

	/// <summary>
	/// 生成(直線)
	/// </summary>
	/// <param name="startPos">生成始点</param>
	/// <param name="endPos">生成終点</param>
	/// <param name="height">生成範囲(高さ)</param>
	/// <param name="maxnum">生成するオブジェクトの最大数</param>
	template <typename T>
	void LinearGenerate(const CVector2& startPos, const CVector2& endPos, float height, int maxnum, float radius = 80.0f) {
		int start = (int)m_genPoints.size();
		
		//生成点作る
		{
			std::unique_ptr<CVector2[]> points = std::make_unique<CVector2[]>(maxnum);
			std::unique_ptr<bool[]> isDead = std::make_unique<bool[]>(maxnum);

			for (int i = 0; i < maxnum; i++) {
				CVector2 pos;
				pos.Lerp((float)i / max(maxnum - 1, 1), startPos, endPos);
				points[i] = pos;
			}

			//点の一定距離内に他の点があったら消滅
			float radiusSq = CMath::Square(radius);
			for (int i = 0; i < maxnum; i++) {
				for (int i2 = i + 1; i2 < maxnum; i2++) {
					if ((points[i] - points[i2]).LengthSq() < radiusSq) {
						isDead[i] = true;//点死亡
						break;
					}
				}
				//引数で渡された点とも判定
				if (!isDead[i]) {
					for (auto& point : m_genPoints) {
						if ((points[i] - point).LengthSq() < radiusSq) {
							isDead[i] = true;//点死亡
							break;
						}
					}
				}
			}

			//統合
			for (int i = 0; i < maxnum; i++) {
				if (!isDead[i]) {//死んでない点
					m_genPoints.emplace_back(points[i]);
				}
			}
		}

		//ステージオブジェクトを作る
		for (int i = start; i < m_genPoints.size(); i++) {
			//座標生成
			CVector3 pos = { m_genPoints[i].x, height, m_genPoints[i].y };
			//レイで判定
			btVector3 rayStart = pos;
			btVector3 rayEnd = btVector3(pos.x, -height, pos.z);
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
	std::vector<CVector2> m_genPoints;
};


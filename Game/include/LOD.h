#pragma once

//LOD(Level of Detail)

/// <summary>
/// LOD表示切り替えオブジェクト
/// </summary>
class ILODObj {
public:
	virtual void SetIsDraw(bool isDraw) = 0;
};
class LODNothing : public ILODObj {
public:
	void SetIsDraw(bool isDraw)override {}
};
class LODInstancingModel : public ILODObj {
public:
	GameObj::CInstancingModelRender& Get() {
		return m_model;
	}
	void SetIsDraw(bool isDraw)override {
		m_model.SetIsDraw(isDraw);
	}
private:
	GameObj::CInstancingModelRender m_model;
};
class LODImposter : public ILODObj {
public:
	CImposter& Get() {
		return m_imposter;
	}
	void SetIsDraw(bool isDraw)override {
		m_imposter.SetIsDraw(isDraw);
	}
private:
	CImposter m_imposter;
};

/// <summary>
/// インスタンシング描画のLOD処理実行クラス
/// </summary>
//class InstancingLOD : public GameObj::InstancingModel::IInstancesData {
//private:
//	//再確保
//	void Reset(int instancingMaxNum) {
//		m_instanceMax = instancingMaxNum;
//		m_isDraw = std::make_unique<bool[]>(instancingMaxNum);
//	}
//public:
//	bool PreCulling(int instanceIndex)override {
//		return m_isDraw[instanceIndex];
//	}
//	void SetInstanceMax(int instanceMax)override {
//		if (instanceMax > m_instanceMax) {
//			Reset(instanceMax);
//		}
//	}
//public:
//	/// <summary>
//	/// コンストラクタ
//	/// </summary>
//	/// <param name="instancingMaxNum">インスタンス最大数</param>
//	InstancingLOD(int instancingMaxNum) {
//		Reset(instancingMaxNum);
//	}
//private:
//	//パラメータ
//	std::unique_ptr<bool[]> m_isDraw;
//	int m_instanceMax = 0;
//};

/// <summary>
/// LOD切り替えクラス
/// ※インスタンシングモデルにのみ対応
/// </summary>
class LODSwitcher : public IGameObject {
public:
	using IGameObject::IGameObject;

	//位置座標を設定
	void SetPos(const CVector3& pos) { m_pos = pos; }

	/// <summary>
	/// 描画物を登録
	/// </summary>
	/// <param name="maxSize">視錐台の最大サイズ</param>
	void AddDrawObject(ILODObj* drawer, float maxSize = FLT_MAX) {
		m_lodObjectList.emplace(maxSize, drawer);
	}

	//処理
	//※このタイミングでSetISDraw更新してもすでにレンダーに登録済み
	//	描画時にGetIsDraw判定
	//　インスタンシングモデルは対応済み
	void Pre3DRender(int num)override {
		bool isDecided = false;
		for (auto i = m_lodObjectList.begin(); i != m_lodObjectList.end(); ++i) {
			CVector2 frustum;
			GetMainCamera()->GetFrustumPlaneSize(GetMainCamera()->GetFront().Dot(m_pos - GetMainCamera()->GetPos()), frustum);
			if (!isDecided && (i == std::prev(m_lodObjectList.end()) || abs(frustum.y) < i->first)) {
				i->second->SetIsDraw(true);
				isDecided = true;
			}
			else {
				i->second->SetIsDraw(false);
			}
		}
	}
	void PreLoopUpdate()override {
		//表示初期化
		for (auto i = m_lodObjectList.begin(); i != m_lodObjectList.end(); ++i) {
			i->second->SetIsDraw(true);
		}
	}

private:
	CVector3 m_pos;//オブジェクトの位置
	std::multimap<float, ILODObj*> m_lodObjectList;
};
#pragma once

//LOD(Level of Detail)

/// <summary>
/// LOD表示切り替えオブジェクト
/// </summary>
class ILODObj {
public:
	virtual void SetIsDraw(bool isDraw) = 0;
};
/// <summary>
/// (LODObj)何も表示しない
/// </summary>
class LODNothing : public ILODObj {
public:
	void SetIsDraw(bool isDraw)override {}
};
/// <summary>
/// (LODObj)インスタンシングモデル
/// </summary>
class LODInstancingModel : public ILODObj {
public:
	LODInstancingModel(bool isRegister = false):m_model(isRegister){}

	GameObj::CInstancingModelRender& Get() {
		return m_model;
	}
	void SetIsDraw(bool isDraw)override {
		m_model.SetIsDraw(isDraw);
	}
private:
	GameObj::CInstancingModelRender m_model;
};
/// <summary>
/// (LODObj)インポスター
/// </summary>
class LODImposter : public ILODObj {
public:
	LODImposter(bool isRegister = false) :m_imposter(isRegister) {}
	
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
/// LOD切り替えクラス
/// ※インスタンシングモデルにのみ対応
/// </summary>
class LODSwitcher : public IGameObject {
public:
	LODSwitcher(bool isRegister = false) : IGameObject(isRegister) {}

	//位置座標を設定
	void SetPos(const CVector3& pos) {
		m_pos = pos;
	}

	/// <summary>
	/// 描画物を登録
	/// </summary>
	void AddDrawObject(ILODObj* drawer, float maxDistance = FLT_MAX) {
		/*if (maxDistance < 0.0f) {
			maxDistance = FLT_MAX;
		}
		else {
			maxDistance = CMath::Square(maxDistance);
		}*/

		int i = 0;
		auto str = m_lodDistanceSq.begin();
		auto end = m_lodDistanceSq.end();
		for (; str != end;++str) {
			if (*str > maxDistance) {
				m_lodDistanceSq.insert(str, maxDistance);
				auto itrObj = m_lodObj.begin() + i;
				m_lodObj.insert(itrObj, drawer);
				return;
			}
			i++;
		}
		m_lodDistanceSq.emplace_back(maxDistance);
		m_lodObj.emplace_back(drawer);
	}

	//処理
	//※このタイミングでSetISDraw更新してもすでにレンダーに登録済み
	//	描画時にGetIsDraw判定
	//　インスタンシングモデルは対応済み
	void Pre3DRender(int num)override {		
		//距離でやる
		//CVector2 frustum;
		//GetMainCamera()->GetFrustumPlaneSize(GetMainCamera()->GetFront().Dot(m_pos - GetMainCamera()->GetPos()), frustum);
		//frustum.y = abs(frustum.y);
		float distanceSq = abs(GetMainCamera()->GetFront().Dot(m_pos - GetMainCamera()->GetPos()));// (m_pos - GetMainCamera()->GetPos()).LengthSq();

		bool isDecided = false;
		const size_t max = m_lodDistanceSq.size();
		for (int i = 0; i < max; ++i) {
			if (!isDecided && (i == max-1 || distanceSq < m_lodDistanceSq[i])) {
				m_lodObj[i]->SetIsDraw(true);
				isDecided = true;
			}
			else {
				m_lodObj[i]->SetIsDraw(false);
			}
		}
	}
	void PreLoopUpdate()override {
		//表示初期化
		for (auto& i : m_lodObj){
			i->SetIsDraw(true);
		}
	}

private:
	CVector3 m_pos;//オブジェクトの位置
	//std::multimap<float, ILODObj*> m_lodObjectList;
	std::vector<float> m_lodDistanceSq;
	std::vector<ILODObj*> m_lodObj;
};
#pragma once

//LOD(Level of Detail)

class ILODObj {
public:
	virtual void SetIsDraw(bool isDraw) = 0;
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

class LODSwitcher : public IGameObject {
public:
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
	//TODO このタイミングでSetISDraw更新してもすでにレンダーに登録済み
	//		レンダー登録より前にすべてのカメラでLOD判定　or 必ず登録
	//		描画時にGetIsDraw判定
	//		インスタン寝具とか無理じゃね
	//		インスタン寝具モデルにisdraw配列もたせて設定　インスタンシングモデルのメンバにインデックスナンバー
	//TODO 画面分割ないと呼ばれない
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

private:
	CVector3 m_pos;//オブジェクトの位置
	std::multimap<float, ILODObj*> m_lodObjectList;
};
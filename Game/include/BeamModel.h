#pragma once
class BeamModel
{
public:
	/// <summary>
	/// ビームの種類
	/// </summary>
	struct BeamType
	{
		wchar_t name[64];
		CVector4 color;
		float emissive = 16.0f;
	};
	static constexpr int BEAM_TYPE_MAXNUM = 6;
	static const BeamType m_s_beamTypes[BEAM_TYPE_MAXNUM];

public:
	BeamModel(const wchar_t* beamName = nullptr, bool isUseInside = true) {
		Init(beamName, isUseInside);
	}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="beamName">ロードするビーム名</param>
	/// <param name="isUseInside">内側の白ビームをびょうがするか?</param>
	void Init(const wchar_t* beamName = nullptr, bool isUseInside = true);

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="rootPos">お尻の座標</param>
	/// <param name="tipPos">頭の座標</param>
	void SetPos(const CVector3& rootPos, const CVector3& tipPos) {
		m_tipPos = tipPos, m_rootPos = rootPos;
		UpdateModel();
	}
	/// <summary>
	/// 玉の半径を設定
	/// </summary>
	void SetRadius(float radius) { 
		m_radius = radius; 
		UpdateModel();
	}
	float GetRadius()const {
		return m_radius;
	}
	/// <summary>
	/// 移動させる
	/// </summary>
	/// <param name="moveVec">移動量</param>
	//void Move(const CVector3& moveVec);

	/// <summary>
	/// 描画するか設定
	/// </summary>
	void SetIsDraw(bool isDraw) {
		int i2 = 0;
		for (int i = 0; i < enBeamModelNum; i++) {			
			if (!m_isUseInside && enInSide == i) { continue; }//内側を描画しない場合
			m_model[i2].SetIsDraw(isDraw);
			i2++;
		}
	}

private:
	/// <summary>
	/// モデルの更新
	/// </summary>
	void UpdateModel();

private:
	static constexpr int BEAM_MODEL_MAXNUM = 512;
	enum enumBeamModel{
		enInSide,
		enOutSide,
		enBeamModelNum
	};
	std::unique_ptr<GameObj::CInstancingModelRender[]> m_model;// [enBeamModelNum];
	bool m_isUseInside = true;

	CVector3 m_tipPos, m_rootPos;
	float m_radius = 1.0f;
};


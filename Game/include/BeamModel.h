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
	};
	static constexpr int BEAM_TYPE_MAXNUM = 3;
	static const BeamType m_s_beamTypes[BEAM_TYPE_MAXNUM];

public:
	BeamModel(const wchar_t** beamName = nullptr) {
		Init(beamName);
	}

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="identifiers">ロードするビーム名</param>
	void Init(const wchar_t** identifiers = nullptr);

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
	/// <summary>
	/// 移動させる
	/// </summary>
	/// <param name="moveVec">移動量</param>
	void Move(const CVector3& moveVec);

private:
	/// <summary>
	/// モデルの更新
	/// </summary>
	void UpdateModel();

private:
	static constexpr int BEAM_MODEL_MAXNUM = 512;
	GameObj::CInstancingModelRender m_model;

	CVector3 m_tipPos, m_rootPos;
	float m_radius = 1.0f;
};


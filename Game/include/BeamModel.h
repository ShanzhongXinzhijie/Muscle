#pragma once
class BeamModel
{
public:
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

	void SetPos(const CVector3& rootPos, const CVector3& tipPos) {
		m_tipPos = tipPos, m_rootPos = rootPos;
		UpdateModel();
	}
	void SetRadius(float radius) { 
		m_radius = radius; 
		UpdateModel();
	}
	void Move(const CVector3& moveVec) {		
		CVector3 soutaiPos = m_tipPos;// -GetMainCamera()->GetPosOld();
		CMatrix viewMat = GetMainCamera()->GetViewMatrixOld();
		viewMat.Mul(soutaiPos);
		//viewMat = GetMainCamera()->GetViewMatrix();
		//viewMat.Transpose();
		//viewMat.Mul(soutaiPos);

		m_rootPos = GetMainCamera()->GetPos() + GetMainCamera()->GetLeft()*-soutaiPos.x + GetMainCamera()->GetUp()*soutaiPos.y + GetMainCamera()->GetFront()*-soutaiPos.z;//
		
		m_tipPos += moveVec;
		UpdateModel();
	}

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


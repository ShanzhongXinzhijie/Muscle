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
	void Init(const wchar_t** identifiers = nullptr) {
		m_model.Init(BEAM_MODEL_MAXNUM, L"Resource/modelData/beam.cmo"
			,nullptr,0,enFbxUpAxisZ,enFbxRightHanded,
			identifiers);
		//F•ÏX
		bool isChanged = false;
		for (auto& type : m_s_beamTypes) {
			if (identifiers && wcscmp(type.name, *identifiers) == 0 || !identifiers && !isChanged) {
				m_model.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
					[&](MaterialSetting* mat) {
						if (mat->EqualMaterialName(L"outside")) {
							mat->SetAlbedoScale(type.color);
							mat->SetEmissive(16.0f);
						}
						else {
							mat->SetEmissive(0.25f);
						}
						mat->SetLightingEnable(false);
					}
				);
				isChanged = true;
			}
		}
		DW_ERRORBOX(!isChanged, "BeamModel::Init() ‚±‚ñ‚ÈF‚Í‚È‚¢");		
	}

	void SetPos(const CVector3& rootPos, const CVector3& tipPos) {
		m_tipPos = tipPos, m_rootPos = rootPos;
		UpdateModel();
	}
	void SetRadius(float radius) { 
		m_radius = radius; 
		UpdateModel();
	}
	void Move(const CVector3& moveVec) {
		m_rootPos = m_tipPos; m_tipPos += moveVec;
		UpdateModel();
	}

private:
	void UpdateModel() {
		m_model.SetPos(m_rootPos);
		CVector3 move = m_rootPos - m_tipPos; move.x *= -1.0f;
		float moveLength = 0.0f;
		if (move.LengthSq() > FLT_EPSILON) {
			CQuaternion rot;
			rot.MakeLookTo(move, CVector3::AxisY());
			m_model.SetRot(rot); 
			moveLength = move.Length();
		}
		m_model.SetScale({ m_radius,m_radius,m_radius + moveLength*0.5f });
	}

private:
	static constexpr int BEAM_MODEL_MAXNUM = 512;
	GameObj::CInstancingModelRender m_model;

	CVector3 m_tipPos, m_rootPos;
	float m_radius = 1.0f;
};


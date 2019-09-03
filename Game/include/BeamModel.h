#pragma once
class BeamModel
{
public:
	/// <summary>
	/// �r�[���̎��
	/// </summary>
	struct BeamType
	{
		wchar_t name[64];
		CVector4 color;
	};
	static constexpr int BEAM_TYPE_MAXNUM = 4;
	static const BeamType m_s_beamTypes[BEAM_TYPE_MAXNUM];

public:
	BeamModel(const wchar_t* beamName = nullptr) {
		Init(beamName);
	}

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="beamName">���[�h����r�[����</param>
	void Init(const wchar_t* beamName = nullptr);

	/// <summary>
	/// ���W��ݒ�
	/// </summary>
	/// <param name="rootPos">���K�̍��W</param>
	/// <param name="tipPos">���̍��W</param>
	void SetPos(const CVector3& rootPos, const CVector3& tipPos) {
		m_tipPos = tipPos, m_rootPos = rootPos;
		UpdateModel();
	}
	/// <summary>
	/// �ʂ̔��a��ݒ�
	/// </summary>
	void SetRadius(float radius) { 
		m_radius = radius; 
		UpdateModel();
	}
	/// <summary>
	/// �ړ�������
	/// </summary>
	/// <param name="moveVec">�ړ���</param>
	void Move(const CVector3& moveVec);

private:
	/// <summary>
	/// ���f���̍X�V
	/// </summary>
	void UpdateModel();

private:
	static constexpr int BEAM_MODEL_MAXNUM = 512;
	enum enumBeamModel{
		enInSide,
		enOutSide,
		enBeamModelNum
	};
	GameObj::CInstancingModelRender m_model[enBeamModelNum];

	CVector3 m_tipPos, m_rootPos;
	float m_radius = 1.0f;
};


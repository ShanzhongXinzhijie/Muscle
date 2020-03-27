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
		float emissive = 16.0f;
	};
	static constexpr int BEAM_TYPE_MAXNUM = 6;
	static const BeamType m_s_beamTypes[BEAM_TYPE_MAXNUM];

public:
	BeamModel(const wchar_t* beamName = nullptr, bool isUseInside = true) {
		Init(beamName, isUseInside);
	}

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="beamName">���[�h����r�[����</param>
	/// <param name="isUseInside">�����̔��r�[�����т傤�����邩?</param>
	void Init(const wchar_t* beamName = nullptr, bool isUseInside = true);

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
	float GetRadius()const {
		return m_radius;
	}
	/// <summary>
	/// �ړ�������
	/// </summary>
	/// <param name="moveVec">�ړ���</param>
	//void Move(const CVector3& moveVec);

	/// <summary>
	/// �`�悷�邩�ݒ�
	/// </summary>
	void SetIsDraw(bool isDraw) {
		int i2 = 0;
		for (int i = 0; i < enBeamModelNum; i++) {			
			if (!m_isUseInside && enInSide == i) { continue; }//������`�悵�Ȃ��ꍇ
			m_model[i2].SetIsDraw(isDraw);
			i2++;
		}
	}

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
	std::unique_ptr<GameObj::CInstancingModelRender[]> m_model;// [enBeamModelNum];
	bool m_isUseInside = true;

	CVector3 m_tipPos, m_rootPos;
	float m_radius = 1.0f;
};


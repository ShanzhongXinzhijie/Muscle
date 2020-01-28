#pragma once

class CShockRing : private SuicideObj::CParticle<CBillboard>
{
public:
	CShockRing(const CVector3& pos,
		const CVector4& color = CVector4::White(),
		const CVector3& scale = 200.0f
	);

private:
	static constexpr int MAX_NUM = 128;//�\���ł���ő吔
	CVector4 m_color;

	//���ʃ��\�[�X
	static inline bool m_isStaticInited;
	static inline Shader m_ps;
};

//�C���X�^���V���O�p�p�����[�^�[
class InstancingShockRingParamManager : public GameObj::InstancingModel::IInstancesData {
private:
	//�Ċm��
	void Reset(int instancingMaxNum);

public:
	void PreDraw(int instanceNum, int drawInstanceNum, const std::unique_ptr<bool[]>& drawInstanceMask)override;
	void AddDrawInstance(int instanceIndex, const CMatrix& SRTMatrix, const CVector3& scale, void *param)override;
	void SetInstanceMax(int instanceMax)override;

public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="instancingMaxNum">�C���X�^���X�ő吔</param>
	InstancingShockRingParamManager(int instancingMaxNum);

private:
	std::unique_ptr<CVector4[]> m_cashe_color;
	StructuredBuffer<CVector4> m_color;

	int m_instanceMax = 0;
};
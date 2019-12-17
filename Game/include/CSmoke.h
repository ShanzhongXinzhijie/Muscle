#pragma once

struct SmokeParam {
	float t = 0.0f;
	CVector4 color = 1.0f;
};

//��
class CSmoke : private SuicideObj::CParticle<CBillboard>
{
public:
	CSmoke(const CVector3& pos, const CVector3& move, 
		   const CVector4& color = CVector4::White(), 
		   const CVector3& scale = 200.0f,
		   const CVector3& scaling = 1.2f
	);

	void Update()override;

private:
	static constexpr int MAX_NUM = 2048;//�\���ł��鉌�̍ő吔

	int m_maxLifeTime = 8;
	SmokeParam m_param;

	//���ʃ��\�[�X
	static bool m_isStaticInited;
	static Shader m_ps;
};

//�C���X�^���V���O�p�p�����[�^�[
class InstancingSmokeParamManager : public GameObj::InstancingModel::IInstancesData {
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
	InstancingSmokeParamManager(int instancingMaxNum);

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_dissolveTextureView;
	std::unique_ptr<float[]> m_cashe_t;
	StructuredBuffer<float> m_dissolve_t;
	std::unique_ptr<CVector4[]> m_cashe_color;
	StructuredBuffer<CVector4> m_color;

	int m_instanceMax = 0;
};


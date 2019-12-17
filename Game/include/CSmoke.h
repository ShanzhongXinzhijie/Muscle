#pragma once

struct SmokeParam {
	float t = 0.0f;
	CVector4 color = 1.0f;
};

//煙
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
	static constexpr int MAX_NUM = 2048;//表示できる煙の最大数

	int m_maxLifeTime = 8;
	SmokeParam m_param;

	//共通リソース
	static bool m_isStaticInited;
	static Shader m_ps;
};

//インスタンシング用パラメーター
class InstancingSmokeParamManager : public GameObj::InstancingModel::IInstancesData {
private:
	//再確保
	void Reset(int instancingMaxNum);

public:
	void PreDraw(int instanceNum, int drawInstanceNum, const std::unique_ptr<bool[]>& drawInstanceMask)override;
	void AddDrawInstance(int instanceIndex, const CMatrix& SRTMatrix, const CVector3& scale, void *param)override;
	void SetInstanceMax(int instanceMax)override;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="instancingMaxNum">インスタンス最大数</param>
	InstancingSmokeParamManager(int instancingMaxNum);

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_dissolveTextureView;
	std::unique_ptr<float[]> m_cashe_t;
	StructuredBuffer<float> m_dissolve_t;
	std::unique_ptr<CVector4[]> m_cashe_color;
	StructuredBuffer<CVector4> m_color;

	int m_instanceMax = 0;
};


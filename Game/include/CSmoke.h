#pragma once

//煙
class CSmoke : private SuicideObj::CParticle<CBillboard>
{
public:
	CSmoke(const CVector3& pos, const CVector3& move, const CVector4& color = CVector4::White());

	void Update()override;

private:
	static constexpr int MAX_NUM = 2048;//表示できる煙の最大数

	int m_maxLifeTime = 8;
	float m_t = 0.0f;

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
	std::unique_ptr<float[]> m_cashe;
	StructuredBuffer<float> m_dissolve_t;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_dissolveTextureView;

	int m_instanceMax = 0;
};


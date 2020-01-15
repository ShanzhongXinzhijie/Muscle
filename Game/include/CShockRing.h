#pragma once

class CShockRing : private SuicideObj::CParticle<CBillboard>
{
public:
	CShockRing(const CVector3& pos,
		const CVector4& color = CVector4::White(),
		const CVector3& scale = 200.0f
	);

private:
	static constexpr int MAX_NUM = 128;//表示できる最大数
	CVector4 m_color;

	//共通リソース
	static inline bool m_isStaticInited;
	static inline Shader m_ps;
};

//インスタンシング用パラメーター
class InstancingShockRingParamManager : public GameObj::InstancingModel::IInstancesData {
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
	InstancingShockRingParamManager(int instancingMaxNum);

private:
	std::unique_ptr<CVector4[]> m_cashe_color;
	StructuredBuffer<CVector4> m_color;

	int m_instanceMax = 0;
};
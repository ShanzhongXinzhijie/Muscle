#include "stdafx.h"
#include "CSmoke.h"
#include "DemolisherWeapon/Graphic/Model/SkinModelShaderConst.h"
#include "shaderDefine.h"

bool CSmoke::m_isStaticInited = false;
Shader CSmoke::m_ps;

CSmoke::CSmoke(
	const CVector3& pos, const CVector3& move,
	const CVector4& color,
	const CVector3& scale, const CVector3& scaling,
	int time
): m_maxLifeTime(time)
{
	if (!m_isStaticInited) {//未初期化
		//ディゾルブシェーダをロード
		D3D_SHADER_MACRO macros[] = { "TEXTURE", "1", "SOFT_PARTICLE", "1", NULL, NULL };
		m_ps.Load("Preset/shader/smoke_u_.fx", "PSMain_Smoke", Shader::EnType::PS, "TEXTURE+SOFT_PARTICLE", macros);		
		//初期化完了
		m_isStaticInited = true;
	}

	//ビルボード読み込み
	std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>();
	billboard->Init(L"Resource/spriteData/smoke.png", MAX_NUM);
	billboard->GetModel().InitPostDraw(PostDrawModelRender::enAlpha,false,true);//ポストドロー(ソフトパーティクル)
	billboard->GetModel().SetIsShadowCaster(false);
	billboard->GetModel().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetPS(&m_ps);//シェーダ設定
		}
	);
	m_param.color = color;

	//IInstanceDataを設定
	if (!billboard->GetInstancingModel().GetInstancingModel()->GetIInstanceData(L"InstancingSmokeParamManager")) {
		//新規作成
		billboard->GetInstancingModel().GetInstancingModel()->AddIInstanceData(L"InstancingSmokeParamManager", std::make_unique<InstancingSmokeParamManager>(MAX_NUM));
	}
	//ポインタを設定
	billboard->GetInstancingModel().SetParamPtr(&m_param);

	//ステータス
	billboard->SetPos(pos);
	billboard->SetScale(scale);
	billboard->SetRot({ CVector3::AxisZ(),CMath::PI2*CMath::RandomZeroToOne() });
	SetMove(move);
	SetScaling(scaling);

	//パーティクル再生
	Play(std::move(billboard), m_maxLifeTime);
}
void CSmoke::Update() {
	m_param.t = 1.0f - (float)GetLifeTime() / m_maxLifeTime;
	SuicideObj::CParticle<CBillboard>::Update();
}


void InstancingSmokeParamManager::Reset(int instancingMaxNum) {
	m_instanceMax = instancingMaxNum;	
	//キャッシュ
	m_cashe_t = std::make_unique<float[]>(m_instanceMax);
	m_cashe_color = std::make_unique<CVector4[]>(m_instanceMax);
	//ストラクチャーバッファ
	m_dissolve_t.Init(m_instanceMax);
	m_color.Init(m_instanceMax);

}
InstancingSmokeParamManager::InstancingSmokeParamManager(int instancingMaxNum) {
	Reset(instancingMaxNum);
	//テクスチャ
	TextureFactory::GetInstance().Load(L"Resource/texture/colud2.png", nullptr, &m_dissolveTextureView, nullptr, true);
}
void InstancingSmokeParamManager::PreDraw(int instanceNum, int drawInstanceNum, const std::unique_ptr<bool[]>& drawInstanceMask) {
	//カリングされてないもののみコピー
	int drawNum = 0;
	std::unique_ptr<float[]>& data_t = m_dissolve_t.GetData();
	std::unique_ptr<CVector4[]>& data_color = m_color.GetData();
	for (int i = 0; i < instanceNum; i++) {
		if (drawInstanceMask[i]) {
			data_t[drawNum] = m_cashe_t[i];
			data_color[drawNum] = m_cashe_color[i];

			//カメラとの距離が近い煙は透明度が高い
			const CMatrix& worldmat = GetDrawInstanceWorldMatrix(drawNum);
			CVector3 pos = { worldmat.m[3][0], worldmat.m[3][1], worldmat.m[3][2] };
			data_color[drawNum].w *= CMath::Clamp((GetMainCamera()->GetPos() - pos).LengthSq() / CMath::Square(10.0f * METER), 0.25f, 1.0f);

			drawNum++;
		}
	}
	//StructuredBufferを更新
	m_dissolve_t.UpdateSubresource();
	m_color.UpdateSubresource();
	//シェーダーリソースにセット
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSkinModelSRVReg_Disolve_t, 1, m_dissolve_t.GetAddressOfSRV()
	);
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSmokeColor, 1, m_color.GetAddressOfSRV()
	);
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSkinModelSRVReg_DisolveTexture, 1, m_dissolveTextureView.GetAddressOf()
	);
}
void InstancingSmokeParamManager::AddDrawInstance(int instanceIndex, const CMatrix& SRTMatrix, const CVector3& scale, void *param) {
	SmokeParam* ptr = (SmokeParam*)param;
	m_cashe_t[instanceIndex] = ptr->t;
	m_cashe_color[instanceIndex] = ptr->color;
}
void InstancingSmokeParamManager::SetInstanceMax(int instanceMax) {
	if (instanceMax > m_instanceMax) {
		Reset(instanceMax);
	}
}
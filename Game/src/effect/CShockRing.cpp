#include "stdafx.h"
#include "CShockRing.h"
#include "shaderDefine.h"

CShockRing::CShockRing(
	const CVector3& pos,
	const CVector4& color,
	const CVector3& scale
){
	if (!m_isStaticInited) {//未初期化
		//シェーダをロード
		D3D_SHADER_MACRO macros[] = { "TEXTURE", "1", "SOFT_PARTICLE", "1", NULL, NULL };
		m_ps.Load("Preset/shader/smoke_u_.fx", "PSMain_ColorChange", Shader::EnType::PS, "TEXTURE+SOFT_PARTICLE", macros);
		//初期化完了
		m_isStaticInited = true;
	}

	//ビルボード読み込み
	std::unique_ptr<CBillboard> billboard = std::make_unique<CBillboard>();
	billboard->Init(L"Resource/effect/airblurring3.png", MAX_NUM);
	billboard->GetModel().InitPostDraw(PostDrawModelRender::enAlpha, false, true);//ポストドロー(ソフトパーティクル)
	billboard->GetModel().SetIsShadowCaster(false);
	billboard->GetModel().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetPS(&m_ps);//シェーダ設定
		}
	);
	//色
	m_color = color;

	//IInstanceDataを設定
	if (!billboard->GetInstancingModel().GetInstancingModel()->GetIInstanceData(L"InstancingShockRingParamManager")) {
		//新規作成
		billboard->GetInstancingModel().GetInstancingModel()->AddIInstanceData(L"InstancingShockRingParamManager", std::make_unique<InstancingShockRingParamManager>(MAX_NUM));
	}
	//ポインタを設定
	billboard->GetInstancingModel().SetParamPtr(&m_color);

	//ステータス
	billboard->SetPos(pos);
	billboard->SetScale(scale);
	billboard->SetRot({ CVector3::AxisZ(),CMath::PI2*CMath::RandomZeroToOne() });
	SetScaling(1.2f);

	//パーティクル再生
	Play(std::move(billboard), 6);
}


void InstancingShockRingParamManager::Reset(int instancingMaxNum) {
	m_instanceMax = instancingMaxNum;
	//キャッシュ
	m_cashe_color = std::make_unique<CVector4[]>(m_instanceMax);
	//ストラクチャーバッファ
	m_color.Init(m_instanceMax);
}
InstancingShockRingParamManager::InstancingShockRingParamManager(int instancingMaxNum) {
	Reset(instancingMaxNum);
}
void InstancingShockRingParamManager::PreDraw(int instanceNum, int drawInstanceNum, const std::unique_ptr<bool[]>& drawInstanceMask) {
	//カリングされてないもののみコピー
	int drawNum = 0;
	std::unique_ptr<CVector4[]>& data_color = m_color.GetData();
	for (int i = 0; i < instanceNum; i++) {
		if (drawInstanceMask[i]) {
			data_color[drawNum] = m_cashe_color[i];
			drawNum++;
		}
	}
	//StructuredBufferを更新
	m_color.UpdateSubresource();
	//シェーダーリソースにセット
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSmokeColor, 1, m_color.GetAddressOfSRV()
	);
}
void InstancingShockRingParamManager::AddDrawInstance(int instanceIndex, const CMatrix& SRTMatrix, const CVector3& scale, void *param) {
	m_cashe_color[instanceIndex] = *(CVector4*)param;
}
void InstancingShockRingParamManager::SetInstanceMax(int instanceMax) {
	if (instanceMax > m_instanceMax) {
		Reset(instanceMax);
	}
}
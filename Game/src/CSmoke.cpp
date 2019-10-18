#include "stdafx.h"
#include "CSmoke.h"
#include "DemolisherWeapon/Graphic/Model/SkinModelShaderConst.h"

bool CSmoke::m_isStaticInited = false;
Shader CSmoke::m_ps;

CSmoke::CSmoke(const CVector3& pos, const CVector3& move, const CVector4& color)
{
	if (!m_isStaticInited) {//未初期化
		//ディゾルブシェーダをロード
		D3D_SHADER_MACRO macros[] = { "TEXTURE", "1", "SOFT_PARTICLE", "1", NULL, NULL };
		m_ps.Load("Preset/shader/modelDisolve.fx", "PSMain_DisolveSozaiNoAzi_ConvertToPMA", Shader::EnType::PS, "TEXTURE+SOFT_PARTICLE", macros);
		
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
			mat->SetAlbedoScale(color);
			mat->SetPS(&m_ps);//シェーダ設定
		}
	);

	//IInstanceDataを設定
	if (!billboard->GetInstancingModel().GetInstancingModel()->GetIInstanceData(L"InstancingSmokeParamManager")) {
		//新規作成
		billboard->GetInstancingModel().GetInstancingModel()->AddIInstanceData(L"InstancingSmokeParamManager", std::make_unique<InstancingSmokeParamManager>(MAX_NUM));
	}
	//ポインタを設定
	billboard->GetInstancingModel().SetParamPtr(&m_t);

	//ステータス
	//TODO
	billboard->SetPos(pos);
	billboard->SetScale(200.0f);
	billboard->SetRot({ CVector3::AxisZ(),CMath::PI2*CMath::RandomZeroToOne() });
	SetMove(move*40);
	SetScaling(1.2f);

	//パーティクル再生
	Play(std::move(billboard), m_maxLifeTime);
}
void CSmoke::Update() {
	m_t = 1.0f - (float)GetLifeTime() / m_maxLifeTime;
	SuicideObj::CParticle<CBillboard>::Update();
}


void InstancingSmokeParamManager::Reset(int instancingMaxNum) {
	m_instanceMax = instancingMaxNum;	
	m_cashe = std::make_unique<float[]>(m_instanceMax);
	m_dissolve_t.Init(m_instanceMax);//ストラクチャーバッファ

}
InstancingSmokeParamManager::InstancingSmokeParamManager(int instancingMaxNum) {
	Reset(instancingMaxNum);
	//テクスチャ
	TextureFactory::GetInstance().Load(L"Resource/texture/colud2.png", nullptr, &m_dissolveTextureView, nullptr, true);
}
void InstancingSmokeParamManager::PreDraw(int instanceNum, int drawInstanceNum, const std::unique_ptr<bool[]>& drawInstanceMask) {
	//カリングされてないもののみコピー
	int drawNum = 0;
	std::unique_ptr<float[]>& data = m_dissolve_t.GetData();
	for (int i = 0; i < instanceNum; i++) {
		if (drawInstanceMask[i]) {
			data[drawNum] = m_cashe[i];
			drawNum++;
		}
	}
	//StructuredBufferを更新
	m_dissolve_t.UpdateSubresource();
	//シェーダーリソースにセット
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSkinModelSRVReg_Disolve_t, 1, m_dissolve_t.GetAddressOfSRV()
	);
	GetGraphicsEngine().GetD3DDeviceContext()->PSSetShaderResources(
		enSkinModelSRVReg_DisolveTexture, 1, m_dissolveTextureView.GetAddressOf()
	);
}
void InstancingSmokeParamManager::AddDrawInstance(int instanceIndex, const CMatrix& SRTMatrix, const CVector3& scale, void *param) {
	m_cashe[instanceIndex] = *(float*)param;
}
void InstancingSmokeParamManager::SetInstanceMax(int instanceMax) {
	if (instanceMax > m_instanceMax) {
		Reset(instanceMax);
	}
}
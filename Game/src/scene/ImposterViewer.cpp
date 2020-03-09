#include "stdafx.h"
#include "ImposterViewer.h"
#include "WindowSizeManager.h"

bool ImposterViewer::Start() {
	//画面サイズ変更
	FindGO<WindowSizeManager>(L"WindowSizeManager")->ChangeWindowSize(false);

	//テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barktex, leaftex;
	TextureFactory::GetInstance().Load(L"Resource/texture/nadeln4.dds", nullptr, &leaftex);
	TextureFactory::GetInstance().Load(L"Resource/texture/stamm2.dds", nullptr, &barktex);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> barktexN, leaftexN;
	TextureFactory::GetInstance().Load(L"Resource/normalMap/nadeln4_n.png", nullptr, &leaftexN, nullptr, true);
	TextureFactory::GetInstance().Load(L"Resource/normalMap/stamm2_n.png", nullptr, &barktexN, nullptr, true);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> leaftexT;
	TextureFactory::GetInstance().Load(L"Resource/translucentMap/nadeln4_t.png", nullptr, &leaftexT, nullptr, true);

	//マテリアル設定関数
	std::function setMaterial = [&](MaterialSetting* me) {
		me->SetShininess(0.01f);
		if (me->EqualMaterialName(L"leaves")) {
			me->SetIsUseTexZShader(true);
			me->SetAlbedoTexture(leaftex.Get());
			me->SetNormalTexture(leaftexN.Get());
			me->SetTranslucentTexture(leaftexT.Get());
			me->SetTranslucent(0.8f);
		}
		else {
			me->SetAlbedoTexture(barktex.Get());
			me->SetNormalTexture(barktexN.Get());
		}
	};

	m_model.Init(L"Resource/modelData/realTree_S.cmo");
	m_model.GetSkinModel().FindMaterialSetting(setMaterial);//マテリアル設定
	m_model.SetPos({ -100.0f,0.f,0.f });

	m_imposter.Init(L"Resource/modelData/realTree_S.cmo", m_model.GetSkinModel(), { 2048, 2048 }, { 7,7 }, 16);
	m_imposter.SetPos({ 100.0f,0.f,0.f });

	m_sprite.Init(m_imposter.GetSRV(ImposterTexRender::enGBufferAlbedo), 2048 , 2048 );
	
	SetMainCamera(&m_camera);

	return true;
}

void ImposterViewer::PostLoopUpdate() {
	//背景
	//DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void ImposterViewer::PostRender() {
	m_sprite.Draw(0.0f, SCREEN_SIZE_2D / (2048.0f));
}

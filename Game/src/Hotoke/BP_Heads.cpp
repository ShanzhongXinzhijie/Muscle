#include "stdafx.h"
#include "BP_Heads.h"

using namespace GameObj;

void BP_HumanHead::InnerStart() {
	m_name = L"ヒューマン";

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/humanhead.cmo");
	//ノーマルマップ適用
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	HRESULT hr = DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/nfish.png", nullptr, textureView.ReleaseAndGetAddressOf());
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.9f);
		}
	);
}

//palmHead
//eggHead
//ELEPHANT_M
//snailHead
//stoneHead
//stoneHead2
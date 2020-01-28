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
	//DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/T_Armour_Clean_Cloth_Normal.png", nullptr, textureView[0].ReleaseAndGetAddressOf());
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/T_Armour_Clean_Metal_Normal.png", nullptr, textureView.ReleaseAndGetAddressOf());
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetMetallic(0.65f);
			//mat->SetShininess(0.9f);
		}
	);
}

void BP_SnailHead::InnerStart() {
	m_name = L"ナメクジかカタツムリ";

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/snailHead.cmo");
	//ノーマルマップ適用
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/Body_Mat_normal.png", nullptr, textureView.ReleaseAndGetAddressOf());
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.75f);
			mat->SetTranslucent(0.2f);
		}
	);
}

void BP_RockHead::InnerStart() {
	m_name = L"イワ";

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/stoneHead.cmo");
	//ノーマルマップ適用
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	//DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/Body_Mat_normal.png", nullptr, textureView.ReleaseAndGetAddressOf());
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			//mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.1f);
		}
	);
}

void BP_RockIIHead::InnerStart() {
	m_name = L"イワ2";

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/stoneHead2.cmo");
	//ノーマルマップ適用
	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	//DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/Body_Mat_normal.png", nullptr, textureView.ReleaseAndGetAddressOf());
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			//mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.1f);
		}
	);
}

void BP_EggHead::InnerStart() {
	m_name = L"タマゴ";

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/eggHead.cmo");
	//マテリアル設定
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetShininess(0.6f);
		}
	);
}

void BP_ElephantHead::InnerStart() {
	m_name = L"エレファント";

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/ELEPHANT_M.cmo");
	//ノーマルマップ適用
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/ELPHSKIN_n.jpeg", nullptr, textureView.ReleaseAndGetAddressOf());
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			if (mat->EqualMaterialName(L"skin")) {
				//皮膚
				mat->SetNormalTexture(textureView.Get());
				mat->SetShininess(0.21f);
			}
			if (mat->EqualMaterialName(L"eye")) {
				//目
				mat->SetShininess(0.9f);
			}
			if (mat->EqualMaterialName(L"tusk")) {
				//牙
			}
		}
	);
}

void BP_PalmHead::InnerStart() {
	m_name = L"ヤシのき";

	//モデル
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/palmHead.cmo");
	//テクスチャロード
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView[2];	
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/palm_bark_norm.png", nullptr, textureView[0].ReleaseAndGetAddressOf());
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/palm leafs_n.png", nullptr, textureView[1].ReleaseAndGetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TranstextureView;
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/translucentMap/palm leafs_t.png", nullptr, TranstextureView.ReleaseAndGetAddressOf());
	//マテリアル設定
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			if (mat->EqualMaterialName(L"palm1_default")) {
				//葉
				mat->SetNormalTexture(textureView[1].Get());
				mat->SetTranslucentTexture(TranstextureView.Get());
				mat->SetTranslucent(0.8f);
				mat->SetShininess(0.165f);
			}
			else {
				//幹
				mat->SetNormalTexture(textureView[0].Get());
				mat->SetShininess(0.01f);
			}
		}
	);
}
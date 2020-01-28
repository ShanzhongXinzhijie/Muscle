#include "stdafx.h"
#include "BP_Heads.h"

using namespace GameObj;

void BP_HumanHead::InnerStart() {
	m_name = L"�q���[�}��";

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/humanhead.cmo");
	//�m�[�}���}�b�v�K�p
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
	m_name = L"�i���N�W���J�^�c����";

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/snailHead.cmo");
	//�m�[�}���}�b�v�K�p
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
	m_name = L"�C��";

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/stoneHead.cmo");
	//�m�[�}���}�b�v�K�p
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
	m_name = L"�C��2";

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/stoneHead2.cmo");
	//�m�[�}���}�b�v�K�p
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
	m_name = L"�^�}�S";

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/eggHead.cmo");
	//�}�e���A���ݒ�
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetShininess(0.6f);
		}
	);
}

void BP_ElephantHead::InnerStart() {
	m_name = L"�G���t�@���g";

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/ELEPHANT_M.cmo");
	//�m�[�}���}�b�v�K�p
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/ELPHSKIN_n.jpeg", nullptr, textureView.ReleaseAndGetAddressOf());
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			if (mat->EqualMaterialName(L"skin")) {
				//�畆
				mat->SetNormalTexture(textureView.Get());
				mat->SetShininess(0.21f);
			}
			if (mat->EqualMaterialName(L"eye")) {
				//��
				mat->SetShininess(0.9f);
			}
			if (mat->EqualMaterialName(L"tusk")) {
				//��
			}
		}
	);
}

void BP_PalmHead::InnerStart() {
	m_name = L"���V�̂�";

	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/palmHead.cmo");
	//�e�N�X�`�����[�h
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView[2];	
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/palm_bark_norm.png", nullptr, textureView[0].ReleaseAndGetAddressOf());
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/palm leafs_n.png", nullptr, textureView[1].ReleaseAndGetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TranstextureView;
	DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/translucentMap/palm leafs_t.png", nullptr, TranstextureView.ReleaseAndGetAddressOf());
	//�}�e���A���ݒ�
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			if (mat->EqualMaterialName(L"palm1_default")) {
				//�t
				mat->SetNormalTexture(textureView[1].Get());
				mat->SetTranslucentTexture(TranstextureView.Get());
				mat->SetTranslucent(0.8f);
				mat->SetShininess(0.165f);
			}
			else {
				//��
				mat->SetNormalTexture(textureView[0].Get());
				mat->SetShininess(0.01f);
			}
		}
	);
}
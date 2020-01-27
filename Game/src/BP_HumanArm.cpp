#include "stdafx.h"
#include "BP_HumanArm.h"

void BP_HumanArm::InnerStart() {
	m_name = L"ヒューマン";
	
	//アニメーション(ボーン初期化用)
	m_initPose.Load(L"Resource/animation/humanarms.tka", true);

	//モデル
	m_model = std::make_unique<GameObj::CSkinModelRender>();
	m_model->Init(L"Resource/modelData/humanarms.cmo", &m_initPose, 1);
	//ノーマルマップ適用
	/*Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	HRESULT hr = DirectX::CreateWICTextureFromFile(GetGraphicsEngine().GetD3DDevice(), L"Resource/normalMap/Crab1NormalsMap.png", nullptr, textureView.ReleaseAndGetAddressOf());
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.7f);
		}
	);*/

	//腕のIK設定
	m_ikSetting[R] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[R]->tipBone = m_model->FindBone(L"Bone006");//013
	m_ikSetting[R]->rootBone = m_model->FindBone(L"Bone003");
	m_ikSetting[R]->iteration = 3;

	m_ikSetting[L] = m_model->GetSkinModel().GetSkeleton().CreateIK();
	m_ikSetting[L]->tipBone = m_model->FindBone(L"Bone008");
	m_ikSetting[L]->rootBone = m_model->FindBone(L"Bone003(mirrored)");
	m_ikSetting[L]->iteration = 3;

	m_ikTargetPos[L] = CVector3::Down()*GetMainCamera()->GetFar();
	m_ikTargetPos[R] = m_ikTargetPos[L];

	////発射口ボーン
	//m_muzzleBoneID[R] = m_model->FindBoneID(L"Bone031");
	//m_muzzleBoneID[L] = m_model->FindBoneID(L"Bone031(mirrored)");

	//コントローラー
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_HumanArm(this, m_ptrCore);
	}
	else {
		m_controller = new AICon_HumanArm(this, m_ptrCore);
	}
}

void BP_HumanArm::Update() {
	//コントローラーに操作させる
	m_controller->Update();
}

void BP_HumanArm::PostUTRSUpdate() {
}

void BP_HumanArm::Draw2D() {
}
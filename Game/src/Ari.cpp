#include "stdafx.h"
#include "Ari.h"

bool Ari::Start() {
	//アニメーション
	m_animation.Load(L"Resource/animation/ari/fly.tka", true);

	//モデル
	m_model.Init(L"Resource/modelData/ari.cmo", &m_animation, 1);
	m_model.SetScale(0.05f);

	//ノーマルマップ 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normaltex;
	TextureFactory::GetInstance().Load(L"Resource/normalMap/n_aritex.png", nullptr, &normaltex, nullptr, true);
	m_model.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(normaltex.Get());
		}
	);

	//コリジョン
	CreateSphere({}, {},50.0f);

	return true;
}

void Ari::Update() {
	//一定時間ごとに移動
	m_moveCnt++;
	if (m_moveCnt >= 60) {
		m_moveCnt = 0;
		//移動量
		CVector3 govec = { 0.0f,0.0f,1.0f };
		CQuaternion rot;
		rot.SetRotation(GetLeft(), CMath::PI2*CMath::RandomZeroToOne());
		rot.Concatenate(CQuaternion(GetUp(), CMath::PI2*CMath::RandomZeroToOne()));
		rot.Multiply(govec);
		//上昇しづらさ
		if (govec.y > 0.0f) { govec.y *= 0.9f; }
		//加速
		m_velocity += govec * 20.0f*1.25f;
		//体の揺れ
		m_shake.SetRotation(CVector3(CMath::RandomZeroToOne()-0.5f, CMath::RandomZeroToOne() - 0.5f, CMath::RandomZeroToOne() - 0.5f).GetNorm(), 0.5f);
	}

	//減速
	m_velocity *= 0.945f;
	m_shake.Slerp(0.1f, m_shake, CQuaternion::Identity());
	//移動
	Move(m_velocity);

	//ターゲット位置に向く
	CQuaternion rot;
	rot.MakeLookToUseXYAxis((GetPos() - m_target->GetPos()).GetNorm());
	SetRot(rot * m_shake);

	//モデル更新
	m_model.SetPos(GetPos());
	m_model.SetRot(GetRot());
}
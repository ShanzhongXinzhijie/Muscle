#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"
#include "BP_HumanMantle.h"

#include "CSmoke.h"
#include "CBlood.h"

void CDeathHotoke::SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part) {
	m_parts[partsType] = std::move(part);
	if (GetIsStart()) { m_parts[partsType]->Start(); }
}

bool CDeathHotoke::Start() {
	//スケール
	constexpr float modelScale = 0.0188f*2.0f;
	m_scale = { modelScale };

	//コア生成
	m_coreModel.Init(L"Resource/modelData/core.cmo");	
	//ノーマルマップ適用
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;	
	TextureFactory::GetInstance().Load(L"Resource/normalMap/ngasi.png", nullptr, &textureView, nullptr, true);
	m_coreModel.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.9f);
			mat->SetTranslucent(0.2f);
		}
	);

	//当たり判定
	CreateCapsule({}, {}, 60.0f*(m_scale.x / (0.0188f*2.0f)), 100.0f*(m_scale.y / (0.0188f*2.0f)));
	SetCollisionFunc(
		[&](ReferenceCollision* H, SuicideObj::CCollisionObj::SCallbackParam& p) {
			CVector3 pos = p.m_collisionPoint;
			Damage(*H,pos);
		}
	);
	SetCollisionPos({ 0.0f, 60.0f*(m_scale.y / (0.0188f*2.0f)), -15.0f*(m_scale.z / (0.0188f*2.0f)) });
	GetAttributes().set(enPhysical);

	//位置初期化	
	SetPos(CVector3::AxisY()*1000.0f + CVector3::AxisZ()*200.0f);
	m_posOld = GetPos();

	//パーツのStart
	for (auto& part : m_parts) {
		if (part)part->Start();
	}
	
	return true;
}

void CDeathHotoke::PreUpdate() {
	//旧座標記録
	m_posOld = GetPos();

	//ステータス更新
	m_drag[enNow] = m_drag[enNext];
	m_angularDrag[enNow] = m_angularDrag[enNext];
	m_rotatability[enNow] = m_rotatability[enNext];
	m_drag[enNext] = 1.0f;
	m_angularDrag[enNext] = 1.0f;
	m_rotatability[enNext] = 1.0f;
}

void CDeathHotoke::Update() {
	//AI実行
	if (m_ai) { m_ai->Update(); }

	//移動適応
	Move(m_veloxity);
	SetRot(m_angularVelocity * GetRot());
	//減速
	m_veloxity *= 0.5f*(1.0f/m_drag[enNow]);
	m_angularVelocity.Slerp(0.5f*(1.0f / m_drag[enNow])*(1.0f / m_angularDrag[enNow]), CQuaternion::Identity(), m_angularVelocity);
	//重力
	m_veloxity.y -= GRAVITY;

	//パーツのUpdate
	for (auto& part : m_parts) {
		if (part)part->Update();	
	}	
	
	//パーツのTRS更新
	for (auto& part : m_parts) {
		if (part)part->UpdateTRS();
	}
	//コアのTRS更新
	m_coreModel.SetPRS(GetPos(), GetRot(), m_scale);
	SetCollisionPos({ 0.0f, 60.0f*(m_scale.y / (0.0188f*2.0f)), -15.0f*(m_scale.z / (0.0188f*2.0f)) });	

	//パーツのワールド行列更新後Update
	for (auto& part : m_parts) {
		if (part)part->PostUTRSUpdate();
	}
}

void CDeathHotoke::PostLoopUpdate() {
	for (auto& part : m_parts) {
		if (part)part->PostLoopUpdate();
	}	
}

void CDeathHotoke::PostRender() {
	//パーツの2D描画
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
	//ステータス描画
	/*if (!m_isDrawHUD) {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"%.1f", m_hp);
		font.Draw(output, { 0.0f,0.25f });
	}
	else {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"(%.1f,%.1f,%.1f)\n%.1f", m_veloxity.x, m_veloxity.y, m_veloxity.z, m_drag[enNow]);
		font.Draw(output, { 0.5f,0.25f }, 1.0f, 0.5f);
	}*/
}

void CDeathHotoke::Damage(const ReferenceCollision& ref, const CVector3& pos) {
	if (ref.damege < FLT_EPSILON) { return; }//ノーダメージ

	m_hp -= ref.damege;

	//TODO
	//出血エフェクト
	//IFu継承をパーティクル化
	//判定あり
	//逆ソフトパーティクルの血痕

	new CSmoke(pos, ref.direction*-1.0f, { 1.0f,0.0f,0.02f,1.0f });
	new CBlood(pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()), (CVector3::Up() + ref.direction*-1.0f)*50.0f);
	new CBlood(pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()), (CVector3::Up() + ref.direction*-1.0f)*50.0f);
}
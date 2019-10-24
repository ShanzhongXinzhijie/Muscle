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
}

void CDeathHotoke::Update() {
	//AI実行
	if (m_ai) { m_ai->Update(); }

	//移動適応
	Move(m_veloxity);
	SetRot(m_rotMove * GetRot());
	//減速
	m_veloxity *= 0.5f;
	m_rotMove.Slerp(0.5f, m_rotMove, CQuaternion::Identity());
	//重力
	m_veloxity.y -= 10.0f;

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
	//パーツのワールド行列更新後アップデート
	for (auto& part : m_parts) {
		if (part)part->PostUTRSUpdate();
	}
}

void CDeathHotoke::PostLoopUpdate() {
	for (auto& part : m_parts) {
		if (part)part->PostLoopUpdate();
	}

	CVector3 origin = GetPos(); origin.y -= 205.0f;
	DrawLine(origin - GetLeft()*100.0f, origin + GetLeft()*100.0f, { 1.0f,0.0f,0.0f,1.0f });
	DrawLine(origin - GetUp()*100.0f, origin + GetUp()*100.0f, { 0.0f,1.0f,0.0f,1.0f });//上方向
	DrawLine(origin - GetFront()*100.0f, origin + GetFront()*100.0f, { 0.0f,0.0f,1.0f,1.0f });
}

void CDeathHotoke::PostRender() {
	//パーツの2D描画
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
	//ステータス描画
	if (!m_isDrawHUD) {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"%.1f", m_hp);
		font.Draw(output, { 0.0f,0.25f });
	}
	else {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"(%.1f,%.1f,%.1f)", m_veloxity.x, m_veloxity.y, m_veloxity.z);
		font.Draw(output, { 0.5f,0.25f });
	}
}

void CDeathHotoke::Damage(const ReferenceCollision& ref, const CVector3& pos) {
	if (ref.damege < FLT_EPSILON) { return; }//ノーダメージ

	m_hp -= ref.damege;

	//TODO
	//出血エフェクト
	//IFu継承をパーティクル化
	//判定あり
	

	new CSmoke(pos, ref.direction*-1.0f, { 1.0f,0.0f,0.02f,1.0f });
	new CBlood(pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()), (CVector3::Up() + ref.direction*-1.0f)*50.0f);
	new CBlood(pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()), (CVector3::Up() + ref.direction*-1.0f)*50.0f);
}
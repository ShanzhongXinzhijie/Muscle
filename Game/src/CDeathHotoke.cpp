#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"
#include "BP_HumanMantle.h"

void CDeathHotoke::CalcDirection() {
	m_front = CVector3::Front(); GetRot().Multiply(m_front); m_back = m_front * -1.0f;
	m_left = CVector3::Left(); GetRot().Multiply(m_left); m_right = m_left * -1.0f;
	m_up = CVector3::Up(); GetRot().Multiply(m_up); m_down = m_up * -1.0f;
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
	m_col.m_collision.CreateMesh({}, {}, m_scale, m_coreModel.GetSkinModel());//TODO 球にしようか
	m_col.m_collision.SetIsHurtCollision(true);//これは喰らい判定
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//クラス取り出す
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				//ダメージ
				Damage(*H);
			}
		}
	);
	
	//位置初期化
	m_pos = CVector3::AxisY()*1000.0f;
	m_pos.z += 200.0f;
	
	//パーツ生成
	m_parts[enWing] = new BP_HumanMantle(this);//BP_BirdWing
	m_parts[enLeg] = new BP_HumanLeg(this);
	m_parts[enArm] = new BP_KaniArm(this);
	m_parts[enHead] = new BP_FishHead(this);
	//パーツのStart
	for (auto& part : m_parts) {
		if (part)part->Start();
	}

	return true;
}

void CDeathHotoke::Update() {
	//AI実行
	if (m_ai) { m_ai->Update(); }

	//移動適応
	m_pos += m_move;
	m_rot = m_rotMove * m_rot;
	//減速
	m_move *= 0.5f;
	m_rotMove.Slerp(0.5f, m_rotMove, CQuaternion::Identity());
	//重力
	m_move.y -= 1.0f;

	//パーツのUpdate
	for (auto& part : m_parts) {
		if (part)part->Update();	
	}	
	//パーツのTRS更新
	for (auto& part : m_parts) {
		if (part)part->UpdateTRS();
	}
	//コアのTRS更新
	m_coreModel.SetPRS(m_pos, m_rot, m_scale);
	//方向ベクトル更新
	CalcDirection();
	//コアのコリジョン更新
	m_col.SetPos(m_pos); m_col.SetRot(m_rot);
	//パーツのワールド行列更新後アップデート
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
	if (!m_isDrawHUD) {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"%.1f", m_hp);
		font.Draw(output, { 0.0f,0.25f });
	}
}

void CDeathHotoke::Damage(const ReferenceCollision& ref) {
	m_hp -= ref.damege;
}
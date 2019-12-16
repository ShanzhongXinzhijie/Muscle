#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"
#include "BP_HumanMantle.h"

#include "CSmoke.h"
#include "CBlood.h"

namespace {
//	float CalcAirScale(float heightMeter) {
//		return max(0.0f, 1.0f - max(0.0f, heightMeter / 1000.0f));//高度1000mに近づくに連れ空気が薄くなる
//	}

	CVector3 CalcCollisionPosOffset(const CVector3& scale) {
		return { 0.0f, 90.0f*(scale.y / (0.0188f*2.0f)), -15.0f*(scale.z / (0.0188f*2.0f)) };
	}
}

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
	CreateCapsule({}, {}, 60.0f*(m_scale.x / (0.0188f*2.0f)), 30.0f*(m_scale.y / (0.0188f*2.0f)));
	SetCollisionFunc(
		[&](ReferenceCollision* H, SuicideObj::CCollisionObj::SCallbackParam& p) {			
			//ダメージ
			Damage(*H, p.m_collisionPoint);
			//スタン
			m_stunTimeSec = max(m_stunTimeSec,H->stunTimeSec);
			//ズームアウト
			if (H->stunTimeSec > 0.0f) {
				CVector3 dir = H->velocity; dir.y = 0.0f; dir.Normalize();
				//CQuaternion(CVector3::AxisY(), CMath::PI_HALF / 2.0f).Multiply(dir);
				m_zoomoutDir = dir * -800.f;
				m_zoomoutDir.y += 400.0f;
			}
		}
	);
	SetPreCollisionFunc(
		[&](ReferenceCollision* H) {
			//体当たり
			auto[damege,stunSec] = DHUtil::CalcRamDamege(GetReferenceCollision().velocity, H->velocity);
			SetDamegePower(damege);//ダメージ	
			SetStunSec(stunSec);//スタン秒数			
			//TODO 体当たりの跳ね返り 多段ヒット
		}
	);
	SetCollisionPosOffset(CalcCollisionPosOffset(m_scale));
	GetAttributes().set(enPhysical);
	//m_col.m_collision.SetIsHighSpeed(true);//これは高速です	

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

	//スタン処理
	Stun();

	//移動適応
	Move(GetTotalVelocity());
	SetRot(m_angularVelocity * GetRot());
	//減速
	m_veloxity *= 0.5f*(1.0f/m_drag[enNow]);
	float linearLength = m_linearVelocity.Length();
	if (linearLength > FLT_EPSILON) {
		m_linearVelocity *= max(0.0f, linearLength - (1.0f + m_drag[enNow]*0.1f)) / linearLength;
	}
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
	SetCollisionPosOffset(CalcCollisionPosOffset(m_scale));
	SetCollisionVelocity(GetMove());

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

void CDeathHotoke::HUDRender(int HUDNum) {
	if (!m_isDrawHUD || m_playerNum != HUDNum)return;

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
	
	//血煙
	new CSmoke(pos, ref.direction*-1.0f* 40.0f, { 1.0f,0.0f,0.02f,1.0f });
	//血飛沫
	int reverseBloodNum = CMath::RandomInt() % 3;//逆方向飛沫の数(0~2)
	for (int i = 0; i < 8; i++) {
		CVector3 move;
		if (i < reverseBloodNum) {
			//逆向き出血
			move = (CVector3::Up() + ref.direction)*(50.0f + 50.0f*CMath::RandomZeroToOne());
		}
		else {
			//通常方向
			move = (CVector3::Up() + ref.direction*-1.0f)*(50.0f + 50.0f*CMath::RandomZeroToOne());
		}
		new CBlood(
			//位置
			pos + CVector3(60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne(), 60.0f - 120.0f*CMath::RandomZeroToOne()),
			//移動
			move
		);
	}
}

void CDeathHotoke::Stun() {
	if (!GetIsStun()) {//スタンしてない
		return;
	}
	m_stunTimeSec -= FRAME_PER_SECOND;

	//錐揉み回転
	AddAngularVelocity(CVector3::AxisY(), CMath::DegToRad(10.0f));
	//落下
	AddVelocity(CVector3::Down()*50.0f);
}

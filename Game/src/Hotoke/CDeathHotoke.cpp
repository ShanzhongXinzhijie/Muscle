#include "stdafx.h"
#include "CDeathHotoke.h"
#include "TimeManager.h"

#include "CSmoke.h"
#include "CBlood.h"

namespace {
	constexpr float modelScale = 0.0188f*2.0f;//モデルのスケール

	CVector3 CalcCollisionPosOffset(const CVector3& scale) {
		return { 0.0f, 90.0f*(scale.y / (0.0188f*2.0f)), -15.0f*(scale.z / (0.0188f*2.0f)) };
	}
}

void CDeathHotoke::SetBodyPart(enBodyParts partsType, std::unique_ptr<IBodyPart> part) {
	m_parts[partsType] = std::move(part);
	m_parts[partsType]->Init(this);
	if (GetIsStart()) { m_parts[partsType]->Start(); }
}

bool CDeathHotoke::Start() {
	//スケール
	m_scale = { modelScale };

	//初期位置
	//レイで判定
	//btVector3 rayStart = GetPos();
	//btVector3 rayEnd = GetPos(); rayEnd.setY(-100000.0f);
	//btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
	//GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
	//if (gnd_ray.hasHit()) {
	//	//接触点を座標に
	//	SetPos(gnd_ray.m_hitPointWorld);
	//}

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

	//光背
	m_kouhai.Init(L"Resource/effect/haikouCliping.png", 1, false);
	m_kouhai.GetModel().GetSkinModel().SetCullMode(D3D11_CULL_NONE);//バックカリングしない
	//m_kouhai.GetModel().InitPostDraw(PostDrawModelRender::enAlpha, false, true);//ポストドロー(ソフトパーティクル)
	m_kouhai.GetModel().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetIsMotionBlur(false);
			mat->SetLightingEnable(false);
		}
	);

	//当たり判定
	CreateCapsule({}, {}, 60.0f*(m_scale.x / (0.0188f*2.0f)), 30.0f*(m_scale.y / (0.0188f*2.0f)));
	
	//ID設定
	SetCollisionOwnerID(GetFuID());
	SetCollisionNonHitID(GetFuID());

	//衝突時に実行する処理
	SetCollisionFunc(
		[&](ReferenceCollision* H, SuicideObj::CCollisionObj::SCallbackParam& p) {			
			//ダメージ
			Damage(*H, p.m_collisionPoint);
			//スタン
			m_stunTimeSec = max(m_stunTimeSec,H->stunTimeSec);
			
			//ズームアウト
			if (H->stunTimeSec > 0.0f) {
				//スローモーション
				constexpr int SLOWMO_FPS = 6;
				constexpr float SLOWMO_SEC = 1.4f;

				//攻撃食らったほう(こっち)
				CVector3 dir;
				dir = H->velocity; dir.y = 0.0f; dir.Normalize();
				if (dir.LengthSq() < FLT_EPSILON) {
					dir = GetFront();
				}
				//CQuaternion(CVector3::AxisY(), CMath::PI_HALF / 2.0f).Multiply(dir);
				m_zoomoutDir = dir * -800.f;
				m_zoomoutDir.y += 400.0f;
				SetZoomoutTime(m_stunTimeSec);
				SetZoomoutTarget(this);				

				//攻撃当てた方の演出
				QueryGOs<CDeathHotoke>(L"CDeathHotoke",
					[&](CDeathHotoke* go) {
						if (go == this) { return true; }
						go->SetZoomoutTime(SLOWMO_SEC * ((FRAME_PER_SECOND * SLOWMO_FPS)));//スローモーが終わるまで
						go->SetZoomoutDirection(m_zoomoutDir);
						go->SetZoomoutTarget(this);
						return true;
					}
				);

				//スローモーション
				TimeManager::GetInstance().SetFrameRate(SLOWMO_SEC, SLOWMO_FPS, 1.0f);
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
			return true;
		}
	);
	SetCollisionPosOffset(CalcCollisionPosOffset(m_scale));//位置
	GetAttributes().set(enPhysical);//物理属性
	GetAttributes().set(enHotokeBody);//ホトケの体属性
	//m_col.m_collision.SetIsHighSpeed(true);//これは高速です	

	//位置初期化	
	m_posOld = GetPos();

	//パーツのStart
	for (auto& part : m_parts) {
		if (part)part->Start();
	}

	//SE
	GetWAVSettingManager().Load(L"Resource/sound/Hit_Hurt.wav")->volume = 0.5f;
	GetWAVSettingManager().Load(L"Resource/sound/Hit_Hurt2.wav")->volume = 0.5f;
	GetWAVSettingManager().Load(L"Resource/sound/Hit_Hurt5.wav")->volume = 0.5f;
	
	return true;
}

void CDeathHotoke::PreLoopUpdate() {
	//描画初期化
	m_kouhai.SetIsDraw(m_isDrawKouhai);
}

void CDeathHotoke::PreUpdate() {
	//旧座標記録
	m_posOld = GetPos();

	//振動音
	if (m_shakePower / 0.0015f > 1.0f) {
		float length = 150.0f;
		switch (CMath::RandomInt() % 3) {
		case 0:
			new GameSE(L"Resource/sound/Hit_Hurt.wav", GetPos(), length, GetPlayerNum());
			break;
		case 1:
			new GameSE(L"Resource/sound/Hit_Hurt2.wav", GetPos(), length, GetPlayerNum());
			break;
		case 2:
			new GameSE(L"Resource/sound/Hit_Hurt5.wav", GetPos(), length, GetPlayerNum());
			break;
		default:
			break;
		}
	}

	//ステータス更新
	m_drag[enNow] = m_drag[enNext];
	m_angularDrag[enNow] = m_angularDrag[enNext];
	m_rotatability[enNow] = m_rotatability[enNext];
	m_drag[enNext] = 1.0f;
	m_angularDrag[enNext] = 1.0f;
	m_rotatability[enNext] = 1.0f;
	m_shakePower = 0.0f;
	m_damegePower *= 0.5f;

	if (m_isInvincible) {
		//表示初期化
		m_coreModel.GetSkinModel().FindMaterialSetting(
			[](MaterialSetting* mat) {
				mat->SetAlbedoScale(CVector4::White());
			}
		);
		//判定初期化
		SetCollisionEnable(true);
		//無敵終了
		m_isInvincible = false;
	}
}

void CDeathHotoke::Update() {
	//AI実行
	if (m_ai) { m_ai->Update(); }

	//スタン処理
	Stun();
	//ズームアウト時間
	m_zoomoutTimeSec -= FRAME_PER_SECOND;
	if (m_zoomoutTimeSec < 0.0f) {
		m_zoomoutTimeSec = 0.0f;
	}

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
	//光背のTRS更新
	m_kouhai.SetPRS(
		GetPos() + (GetBack() * 25000.0f*0.5f * m_scale) + (GetUp() * 25000.0f*0.25f * m_scale),
		GetRot(),
		50000.0f* 0.75f * m_scale
	);

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

void CDeathHotoke::Pre3DRender(int screenNum) {
	//光背モデルを相手の画面でのみ描画させる
	if (!m_isDrawKouhai) {
		return;
	}
	if (m_playerNum != screenNum) {
		m_kouhai.SetIsDraw(true);
	}
	else {
		m_kouhai.SetIsDraw(false);
	}
}

void CDeathHotoke::HUDRender(int HUDNum) {
	if (!m_isDrawHUD || m_playerNum != HUDNum)return;

	if (GetIsDrawHowto()) {
		//操作
		if (GetPlayerNum() == 0) {
			GetJapaneseFont()->Draw(L"[Rスティック]\n:してんいどう\n[RSB]バックみる", { 0.0f,0.15f });
		}
		else {
			GetJapaneseFont()->Draw(L"[Rスティック]\n:してんいどう\n[RSB]バックみる", { 1.0f,0.15f }, { 1.1f,0.0f });
		}
	}

	//パーツの2D描画
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
}

void CDeathHotoke::Damage(const ReferenceCollision& ref, const CVector3& pos) {
	if (ref.damege < FLT_EPSILON) { return; }//ノーダメージ

	if (!m_isImmortal) {
		m_hp -= ref.damege;
		m_damegePower = max(ref.damege, m_damegePower);
	}

	//SE
	new GameSE(L"Resource/sound/Explosion6.wav", GetPos(), 1000.0f, GetPlayerNum());

	//血煙
	for (int i = 0; i < 3; i++) {
		CVector3 dir = ref.direction*-1.0f*(40.0f + 400.0f*CMath::RandomZeroToOne());
		CQuaternion(CVector3::AxisX(), CMath::PI_HALF*0.25f - CMath::PI_HALF*0.5f*CMath::RandomZeroToOne()).Multiply(dir);
		CQuaternion(CVector3::AxisY(), CMath::PI_HALF*0.25f - CMath::PI_HALF*0.5f*CMath::RandomZeroToOne()).Multiply(dir);
		new CSmoke(pos, dir, BLOOD_BLACK_COLOR, 200.0f, 1.2f + 0.1f*(1.0f+CMath::RandomZeroToOne()));
	}
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

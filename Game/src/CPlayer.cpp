#include "stdafx.h"
#include "CPlayer.h"
#include "CGameMode.h"

HumanPlayer::HumanPlayer(int playernum, CDeathHotoke& hotoke):
	//プレイヤー番号
	m_playerNum(playernum),
	//ホトケ参照
	m_hotoke(hotoke),
	//パット
	m_pad(playernum),
	//カメラ
	m_cam(&m_hotoke, &m_pad),
	//フォント
	m_HUDFont(m_HUDColor, 0.5f), m_warningFont(CVector4::Red(), 0.5f), m_japaneseFont(m_HUDColor, 0.5f) 
{
	//メインカメラ設定
	m_cam.SetToMainCamera(playernum);

	//草
	for (auto& grass : m_grass) {
		grass.SetDrawCameraNum(playernum);
	}

	//フォント
	m_japaneseFont.SetUseFont(HUDFont::enJPN);
	m_hotoke.SetFonts(&m_warningFont, &m_japaneseFont);
}

CPlayer::CPlayer(int playernum) :
	//プレイヤー番号
	m_playerNum(playernum),
	//阿泉設定
	m_assemble(FindGO<HotokeAssembleManager>(L"HotokeAssembleManager")->GetHotokeAssemble(m_playerNum))	
{
	//人間用データ
	if (!m_assemble.ai) {
		m_humanPlayer = std::make_unique<HumanPlayer>(playernum, m_hotoke);
	}

	//本体
	if (!m_assemble.ai) {
		//プレイヤー
		m_hotoke.Init(playernum, &m_humanPlayer->GetPad(), true, &m_humanPlayer->GetHUDFont(), nullptr);
	}
	else {
		//CPU
		m_hotoke.Init(playernum, nullptr, false, nullptr, std::unique_ptr<IAI>(m_assemble.ai->Create(&m_hotoke)));
	}	
};

bool CPlayer::Start() {
	//機体のパーツを設定
	m_hotoke.SetBodyPart(CDeathHotoke::enHead, std::unique_ptr<IBodyPart>(m_assemble.parts[CDeathHotoke::enHead]->Create()));
	m_hotoke.SetBodyPart(CDeathHotoke::enArm, std::unique_ptr<IBodyPart>(m_assemble.parts[CDeathHotoke::enArm]->Create()));
	m_hotoke.SetBodyPart(CDeathHotoke::enWing, std::unique_ptr<IBodyPart>(m_assemble.parts[CDeathHotoke::enWing]->Create()));
	m_hotoke.SetBodyPart(CDeathHotoke::enLeg, std::unique_ptr<IBodyPart>(m_assemble.parts[CDeathHotoke::enLeg]->Create()));

	return true;
}

bool HumanPlayer::Start() {

	if (m_playerNum == 0) {
		//テストモデル
		m_animHeri.Load(L"Resource/animation/herico.tka", true);
		m_heri.Init(L"Resource/modelData/herico.cmo", &m_animHeri, 1);
		m_heri.SetScale(0.3f);
		m_heri.GetSkinModel().FindMaterialSetting([](MaterialSetting* mat) {mat->SetAlbedoScale({ 0.01f,0.01f,0.05f,1.0f }); });

		//レイで判定
		btVector3 rayStart = btVector3(0.0f, 100000.0f, 0.0f);
		btVector3 rayEnd = btVector3(0.0f, -100000.0f, 0.0f);
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			//接触点を座標に
			m_heri.SetPos(gnd_ray.m_hitPointWorld + CVector3::Back()*25.f + CVector3::Up() * 1250.0f);
		}
	}

	m_humanPtr = FindGO<CHuman>(L"CHuman");
	m_gameModePtr = FindGO<CGameMode>(L"CGameMode");

	//HUD
	m_guncross.Init(L"Resource/spriteData/gunCross.png");
	m_wMark.Init(L"Resource/spriteData/wMark.png");
	m_velocityVector.Init(L"Resource/spriteData/velocityVector.png");

	return true;
}

void HumanPlayer::Update() {

#ifndef DW_MASTER
	//デバッグ移動
	CVector3 pos = m_hotoke.GetPos();
	CVector3 moveDir;
	if (GetKeyInput('W')) {
		moveDir.z += 2.0f;
	}
	if (GetKeyInput('S')) {
		moveDir.z -= 2.0f;
	}
	if (GetKeyInput('A')) {
		moveDir.x -= 2.0f;
	}
	if (GetKeyInput('D')) {
		moveDir.x += 2.0f;
	}
	moveDir.y = 0.0f;
	moveDir.Normalize();
	if (GetKeyInput(VK_LSHIFT)) {
		pos += moveDir * 1.0f;
	}
	else {
		pos += moveDir * 10.0f;
	}
	if (GetKeyInput(VK_SPACE)) {
		pos.y += 40.0f;
	}
	if (GetKeyInput('C')) {
		pos.y -= 20.0f;
	}
	m_hotoke.SetPos(pos);
#endif

	//カメラ切り替え
	if (GetKeyInput('F') || Pad(m_playerNum).GetButton(enButtonBack)) {
		//ヒト目線
		m_humanPtr->EnableHumanCamera(m_hotoke.GetPos(), m_playerNum);
	}
	else {
		//元
		m_humanPtr->DisableHumanCamera();
		m_cam.SetToMainCamera();
		m_cam.SetToMainCamera(m_playerNum);
	}

	//引きカメラ演出
	m_cam.SetIsZoomout(m_hotoke.GetIsStun(), m_hotoke.GetZoomoutDirection());

	//消失点
	CVector3 vanisingPoint = m_hotoke.GetPos() + m_hotoke.GetTotalVelocity() + (m_cam.GetTargetPoint() - m_hotoke.GetPos()).GetNorm()*15000.0f*0.125f;
	m_hotoke.SetVanisingPoint(vanisingPoint);

	//ロックオン
	bool isLock = false; float minDistance = 0.0f; int lastLockLevel = -1;
	CVector3 outPos; LockableWrapper* outObj = nullptr;
	QueryGOs<LockableWrapper>(L"LockableObject", [&](LockableWrapper* go) {
		if (&m_hotoke == go->GetFu() || &m_hotoke == go->GetOwner()) { return true; }//自分は除く

		CVector3 screenPos = m_cam.CalcScreenPosFromWorldPos(go->GetFu()->GetCollisionPos());
		float distance = CVector3(screenPos.x - 0.5f, screenPos.y - 0.5f, 0.0f).LengthSq();
		//位置が画面内か?
		if (screenPos.x > 0.0f && screenPos.x < 1.0f && screenPos.y > 0.0f && screenPos.y < 1.0f && screenPos.z > 0.0f && screenPos.z < 1.0f) {
			if (!isLock || lastLockLevel < go->GetPriorityLevel() || minDistance > distance) {
				isLock = true;
				outPos = go->GetFu()->GetCollisionPos();
				outObj = go;
				screenPos.z = 0.0f; minDistance = distance;
			}
		}
		return true; 
	});
	//ホトケのターゲット位置設定
	if (isLock) {
		m_hotoke.SetTargetPos(outPos);
		m_hotoke.SetTarget(outObj);
		m_targetHP = m_hotoke.GetTarget()->GetHP();
		m_targetPos = m_hotoke.GetTargetFu()->GetPos();
	}
	else {
		m_hotoke.SetTargetPos(vanisingPoint);
		m_hotoke.SetTarget(nullptr);
	}
	m_isLockon = isLock;
}

void HumanPlayer::PostUpdate() {
	m_hotoke.SetIsBackMirror(m_cam.GetIsBackMirror());//ホトケにバックミラー状態を通知
}

namespace {
	const CVector3 outlineOffset = { 0.001f,0.001f,0.0f };
	constexpr float HPBarLength = 0.4f;
	const CVector3 HPBarMin = { 0.3f, 0.95f, 0.0f };
	const CVector3 HPBarMax = { 0.3f + HPBarLength, 0.95f + 0.005f,0.0f };
}

void HumanPlayer::PostLoopUpdate() {
	if (!m_isDrawHUD)return;
	if (GetKeyInput('F')) { return; }

	//HealthBar
	DrawQuad2D(HPBarMin - outlineOffset, HPBarMax + outlineOffset, { 1.0f - m_HUDColor.x, 1.0f - m_HUDColor.y, 1.0f - m_HUDColor.z, m_HUDColor.w*0.5f }, m_playerNum);
	DrawQuad2D(HPBarMin, HPBarMax - CVector3::AxisX()*HPBarLength*(1.0f-m_hotoke.GetHPPer()), m_HUDColor, m_playerNum);

	//エネミーへのライン
	QueryGOs<LockableWrapper>(L"LockableObject", [&](LockableWrapper* go) {
		if (&m_hotoke == go->GetFu() || &m_hotoke == go->GetOwner()) { return true; }//自分は除く

		CVector3 screenPos = m_cam.CalcScreenPosFromWorldPos(go->GetFu()->GetCollisionPos());
		
		//位置が画面内か?
		if (screenPos.x > 0.0f && screenPos.x < 1.0f && screenPos.y > 0.0f && screenPos.y < 1.0f && screenPos.z > 0.0f && screenPos.z < 1.0f) {
			DrawLine2D({0.5f}, screenPos, m_enemyColor*CVector4(1.0f, 1.0f, 1.0f, 0.5f), m_playerNum);
		}
		return true;
	});
	
	//グリッド
	CVector3 origin = m_cam.GetPos(); origin += m_hotoke.GetFront()*380.0f;
	//DrawLine3D(origin - m_hotoke.GetLeft()*100.f, origin + m_hotoke.GetLeft()*100.f, m_HUDColor, m_playerNum);
	//DrawLine3D(origin - m_hotoke.GetUp()*100.f, origin + m_hotoke.GetUp()*100.f, m_HUDColor, m_playerNum);//上方向
	
	origin = m_cam.CalcScreenPosFromWorldPos(origin);
	if (origin.z > 0.0f && origin.z < 1.0f) {
		DrawLine2D(origin + CVector3(-0.2f, 0.0f, 0.0f), origin + CVector3(0.2f, 0.0f, 0.0f), m_HUDColor*CVector4(1.0f, 1.0f, 1.0f, 0.5f), m_playerNum);
		DrawLine2D(origin + CVector3(0.0f, -0.2f, 0.0f), origin + CVector3(0.0f, 0.2f, 0.0f), m_HUDColor*CVector4(1.0f, 1.0f, 1.0f, 0.5f), m_playerNum);
	}
}

void HumanPlayer::HUDRender(int HUDNum) {
	if (!m_isDrawHUD || m_playerNum != HUDNum)return;
	if (GetKeyInput('F')) { return; }

	//Health
	m_HUDFont.DrawFormat(L"%.0f/%.0f", HPBarMax + outlineOffset, { 1.0f,0.0f }, m_hotoke.GetHP(), m_hotoke.GetHPMax());

	//期待の向いている方の位置
	CVector3 tdFrontPos = m_cam.CalcScreenPosFromWorldPos(m_cam.GetPos() + m_hotoke.GetFront()*380.0f);
	
	//速度
	if (tdFrontPos.z > 0.0f && tdFrontPos.z < 1.0f) {
		float kmh;
		//時速
		kmh = m_hotoke.GetMove().Length()*FRAME_RATE*60.0f*60.0f / METER / 1000.0f;
		m_HUDFont.DrawFormat(L"%.0f", tdFrontPos - CVector3(0.06f,  0.025f, 0.0f), { 1.0f,1.0f }, kmh);
		
		//落下速度
		kmh = -m_hotoke.GetMove().y*FRAME_RATE*60.0f*60.0f / METER / 1000.0f;
		if (abs(kmh) < 0.1f) { 
			kmh = 0.0f; 
		}
		m_HUDFont.DrawFormat(L"%.0f", tdFrontPos - CVector3(0.06f, 0.0f, 0.0f), { 1.0f,0.0f }, kmh);

		//高度
		m_HUDFont.DrawFormat(L"%.0f", tdFrontPos - CVector3(-0.06f, 0.025f, 0.0f), { 0.0f,1.0f }, m_hotoke.GetHeightMeter());
	}
	
	CVector3 pos;

	//ウイスキーマーク(機体の向き)
	pos = tdFrontPos;
	if (pos.z > 0.0f && pos.z < 1.0f) {
		m_wMark.Draw(pos, 1.0f, 0.5f, 0.0f, m_HUDColor);
	}
	
	//ガンクロス(照準)
	pos = m_cam.CalcScreenPosFromWorldPos(m_hotoke.GetTargetPos());
	if (pos.z > 0.0f && pos.z < 1.0f) { 
		/*if (m_guncrossPosOld.z > 0.0f && m_guncrossPosOld.z < 1.0f) {
			constexpr int loopmax = 8;
			for (int i = 0; i < loopmax; i++) {
				m_guncross.Draw(pos + (m_guncrossPosOld - pos) / loopmax * (i+1), 1.0f, 0.5f, m_isLockon ? 0.0f : CMath::PI_QUARTER, m_HUDColor*CVector4(1.0f, 1.0f, 1.0f, 1.0f));
			}
		}*/
		if (m_isLockon) {
			m_guncross.Draw(pos, 1.0f, 0.5f, m_isLockon ? 0.0f : CMath::PI_QUARTER, m_enemyColor);
			//ステータス
			CVector4 color = m_HUDFont.GetColor();
			m_HUDFont.SetColor(m_enemyColor);
			
			//HP
			if (m_targetHP > 0.0f) {
				m_HUDFont.DrawFormat(L"%.0f", pos - CVector3(0.0f, 0.025f, 0.0f), { 0.5f,1.5f }, m_targetHP);
			}
			//距離(m)
			CVector2 motoScale = m_HUDFont.GetScale();
			m_HUDFont.SetScale(motoScale*0.75f);
			m_HUDFont.DrawFormat(L"%.0f", pos - CVector3(0.0f, -0.025f, 0.0f), { 0.5f,0.0f }, (m_targetPos - m_hotoke.GetPos()).Length() / METER);
			m_HUDFont.SetScale(motoScale);
			
			m_HUDFont.SetColor(color);
		}
		else {
			m_guncross.Draw(pos, 1.0f, 0.5f, m_isLockon ? 0.0f : CMath::PI_QUARTER, m_HUDColor);
		}
	}
	//m_guncrossPosOld = pos;

	//ベロシティベクトル(進行方向)
	if (m_hotoke.GetMove().LengthSq() > 1.0f) {
		pos = m_cam.CalcScreenPosFromWorldPos(m_hotoke.GetPos() + m_hotoke.GetMove().GetNorm() * (m_cam.GetFar()*0.5f));
		if (pos.z > 0.0f && pos.z < 1.0f) { m_velocityVector.Draw(pos, 0.75f, 0.5f, 0.0f, m_HUDColor); }
	}
	//m_velocityPosOld = pos;

	//バックミラー
	if (m_cam.GetIsBackMirror()) {
		m_HUDFont.Draw(L"((BACK VIEW))", { 0.5f,0.0f }, { 0.5f,0.0f });
	}
}
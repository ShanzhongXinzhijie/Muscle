#include "stdafx.h"
#include "CPlayer.h"
#include "CGameMode.h"
#include "TreeGene.h"

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
	m_HUDFont(m_HUDColor, 0.5f), m_warningFont({255.f/255.f,48.f/255.f,0.f,1.f}, 0.5f), m_japaneseFont(m_HUDColor, 0.5f)
{
	//メインカメラ設定
	m_cam.SetToMainCamera(playernum);

	//フォント
	m_japaneseFont.SetUseFont(HUDFont::enJPN);
	m_hotoke.SetFonts(&m_warningFont, &m_japaneseFont);
}

CPlayer::CPlayer(int playernum, bool isNoAI) :
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
		m_hotoke.Init(playernum, nullptr, false, nullptr, isNoAI ? std::make_unique<DummyAI>(&m_hotoke) : std::unique_ptr<IAI>(m_assemble.ai->Create(&m_hotoke)));
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
	//参照
	m_humanPtr = FindGO<CHuman>(L"CHuman");

	//HUD
	m_guncross.Init(L"Resource/spriteData/gunCross.png");
	m_lockMark.Init(L"Resource/spriteData/lockon.png");
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
	if (GetKeyDown('F') || Pad(m_playerNum).GetDown(enButtonBack)) {
		m_isHumanCamera = !m_isHumanCamera;
		m_hotoke.SetIsDrawHUD(!m_isHumanCamera);	
		m_hotoke.SetIsForceDrawKouhai(m_isHumanCamera);
	}
	if (m_isHumanCamera) {
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
	m_cam.SetIsZoomout(m_hotoke.GetIsZoomOut(), m_hotoke.GetZoomoutDirection(), m_hotoke.GetZoomoutTarget());

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
		//SE
		if (m_hotoke.GetTarget() != outObj) {
			new GameSE(L"Resource/sound/lock.wav", m_hotoke.GetPos(), 150.0f, m_hotoke.GetPlayerNum());
		}
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
	if (!m_isDrawHUD || m_isHumanCamera)return;

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
		DrawLine2D(origin + CVector3(0.0f, -0.0f, 0.0f), origin + CVector3(0.0f, 0.2f, 0.0f), m_HUDColor*CVector4(1.0f, 1.0f, 1.0f, 0.5f), m_playerNum);
	}

	//振動
	if (m_hotoke.GetShakePower() > 0.0f) {
		//for (int i = 0; i < 4; i++) {せ
			DrawQuad2D({ 0.0f,0.0f,0.0f }, { 1.0f * CMath::RandomZeroToOne() * m_hotoke.GetShakePower(),1.0f,0.0f }, CVector4(0.0f, 0.0f, 0.0f, 0.7f), m_playerNum);
			DrawQuad2D({ 1.0f - 1.0f * CMath::RandomZeroToOne() * m_hotoke.GetShakePower(),0.0f,0.0f }, { 1.0f,1.0f,0.0f }, CVector4(0.0f, 0.0f, 0.0f, 0.7f), m_playerNum);
			DrawQuad2D({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f * CMath::RandomZeroToOne() * m_hotoke.GetShakePower(),0.0f }, CVector4(0.0f, 0.0f, 0.0f, 0.7f), m_playerNum);
			DrawQuad2D({ 0.0f,1.0f - 1.0f * CMath::RandomZeroToOne() * m_hotoke.GetShakePower(),0.0f }, { 1.0f,1.0f,0.0f }, CVector4(0.0f, 0.0f, 0.0f, 0.7f), m_playerNum);
		//}
	}
	//ダメージ
	if (m_hotoke.GetDamegePower() > 0.0f) {
		float power = m_hotoke.GetDamegePower()*0.1f;
		DrawQuad2D({ 0.0f,0.0f,0.0f }, { 1.0f * CMath::RandomZeroToOne() * power,1.0f,0.0f }, CVector4(1.0f, 0.0f, 0.0f, 0.3f), m_playerNum);
		DrawQuad2D({ 1.0f - 1.0f * CMath::RandomZeroToOne() * power,0.0f,0.0f }, { 1.0f,1.0f,0.0f }, CVector4(1.0f, 0.0f, 0.0f, 0.3f), m_playerNum);
		DrawQuad2D({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f * CMath::RandomZeroToOne() * power,0.0f }, CVector4(1.0f, 0.0f, 0.0f, 0.3f), m_playerNum);
		DrawQuad2D({ 0.0f,1.0f - 1.0f * CMath::RandomZeroToOne() * power,0.0f }, { 1.0f,1.0f,0.0f }, CVector4(1.0f, 0.0f, 0.0f, 0.3f), m_playerNum);
	}
}

void HumanPlayer::HUDRender(int HUDNum) {
	if (!m_isDrawHUD || m_isHumanCamera || m_playerNum != HUDNum)return;

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
			m_lockMark.Draw(pos, 1.0f, 0.5f, m_isLockon ? 0.0f : CMath::PI_QUARTER, m_enemyColor);//*CVector4(1.0f,1.0f,1.0f,0.5f)
			//ステータス
			CVector4 color = m_HUDFont.GetColor();
			CVector2 motoScale = m_HUDFont.GetScale();
			
			m_HUDFont.SetColor(m_enemyColor);
			
			//HP
			if (m_targetHP > 0.0f) {
				m_HUDFont.DrawFormat(L"%.0f", pos - CVector3(0.0f, 0.025f, 0.0f), { 0.5f,1.5f }, m_targetHP);
			}
			//距離(m)
			m_HUDFont.SetScale(motoScale*0.75f);
			//m_HUDFont.SetAlpha(0.0f);
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
#include "stdafx.h"
#include "CPlayer.h"

#include "BP_FishHead.h"
#include "BP_BirdWing.h"
#include "BP_KaniArm.h"
#include "BP_HumanLeg.h"
#include "BP_HumanMantle.h"

bool CPlayer::Start() {	
	m_anim.Load(L"Resource/animation/human/stand.tka");
	m_human.Init(L"Resource/modelData/human.cmo", &m_anim, 1);
	//m_human.SetPos(CVector3::AxisY()*900.0f + CVector3::AxisX()*50.0f + CVector3::AxisZ()*100.0f);
	m_human.SetScale(10.0f);

	//m_humanCam.SetViewAngleDeg(25.0f);
	m_humanCam.SetFar(15000.0f);

	m_hotoke.SetBodyPart(CDeathHotoke::enHead, std::make_unique<BP_FishHead>(&m_hotoke));
	m_hotoke.SetBodyPart(CDeathHotoke::enArm, std::make_unique<BP_KaniArm>(&m_hotoke));
	m_hotoke.SetBodyPart(CDeathHotoke::enWing, std::make_unique<BP_BirdWing>(&m_hotoke));
	m_hotoke.SetBodyPart(CDeathHotoke::enLeg, std::make_unique<BP_HumanLeg>(&m_hotoke));

	//HUD
	m_guncross.Init(L"Resource/spriteData/gunCross.png");
	m_wMark.Init(L"Resource/spriteData/wMark.png");
	m_velocityVector.Init(L"Resource/spriteData/velocityVector.png");

	return true;
}

void CPlayer::Update() {

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

	//カメラ切り替え
	static bool uih = false;
	static bool iscahsnge = false;
	if (GetKeyInput('F')) {
		m_humanCam.SetPos(m_human.GetBonePos(m_human.FindBoneID(L"Head"))+CVector3::AxisY()*0);
		m_humanCam.SetTarget(m_hotoke.GetPos());
		SetMainCamera(&m_humanCam);

		if (!uih) {
			constexpr int screenSize = 640;
			constexpr int screenSize3D = 640;
			constexpr int HUDSize = 64;
			if (iscahsnge) {
				GetEngine().ChangeWindowSize(screenSize, screenSize);
				GetGraphicsEngine().ChangeFrameBufferSize(screenSize, screenSize, screenSize3D, screenSize3D, enNoSplit);
				iscahsnge = false;
			}
			else {
				GetEngine().ChangeWindowSize(screenSize*2+ HUDSize, screenSize);
				float screensSize[8] = {
					0.0f,0.0f,(float)screenSize / (screenSize * 2 + HUDSize),1.0f,
					1.0f - (float)screenSize / (screenSize * 2 + HUDSize),0.0f,1.0f,1.0f,
				};
				GetGraphicsEngine().ChangeFrameBufferSize(screenSize*2+ HUDSize, screenSize, screenSize3D *2+ HUDSize, screenSize3D, enSide_TwoSplit, screensSize);
				iscahsnge = true;
			}
		}
		uih = true;
	}
	else {
		m_cam.SetToMainCamera();
		uih = false;
	}

	//ロックオン
	bool isLock = false; float minDistance = 0.0f; 
	CVector3 outPos; IFu* outFu = nullptr;
	QueryGOs<IFu>(L"LockableObject", [&](IFu* go) {
		if (&m_hotoke == go) { return true; }//自分は除く

		CVector3 screenPos = m_cam.CalcScreenPosFromWorldPos(go->GetCollisionPos());
		float distance = CVector3(screenPos.x - 0.5f, screenPos.y - 0.5f, 0.0f).LengthSq();
		//位置が画面内か?
		if (screenPos.x > 0.0f && screenPos.x < 1.0f && screenPos.y > 0.0f && screenPos.y < 1.0f && screenPos.z > 0.0f && screenPos.z < 1.0f) {
			if (!isLock || minDistance > distance) {
				isLock = true;
				outPos = go->GetCollisionPos();
				outFu = go;
				screenPos.z = 0.0f; minDistance = distance;
			}
		}
		return true; 
	});
	//ホトケのターゲット位置設定
	if (isLock) {
		m_hotoke.SetTargetPos(outPos);
		m_hotoke.SetTargetFu(outFu);
	}
	else {
		m_hotoke.SetTargetPos(m_cam.GetTargetPoint());
		m_hotoke.SetTargetFu(nullptr);
	}
	m_isLockon = isLock;
}

void CPlayer::PostUpdate() {
	m_hotoke.SetIsBackMirror(m_cam.GetIsBackMirror());
}

void CPlayer::PostLoopUpdate() {
	if (!m_isDrawHUD)return;

	//グリッド
	CVector3 origin = m_cam.GetPos(); origin += m_hotoke.GetFront()*380.0f;
	DrawLine(origin - m_hotoke.GetLeft()*m_cam.GetFar(), origin + m_hotoke.GetLeft()*m_cam.GetFar(), m_HUDColor);
	DrawLine(origin - m_hotoke.GetUp()*m_cam.GetFar(), origin + m_hotoke.GetUp()*m_cam.GetFar(), m_HUDColor);//上方向
	//origin = m_cam.GetPos(); origin -= m_hotoke.GetFront()*1000.0f;
	//DrawLine(origin - m_hotoke.GetLeft()*m_cam.GetFar()*2.0f, origin + m_hotoke.GetLeft()*m_cam.GetFar()*2.0f, m_HUDColor);
	//DrawLine(origin - m_hotoke.GetUp()*m_cam.GetFar()*2.0f, origin + m_hotoke.GetUp()*m_cam.GetFar()*2.0f, m_HUDColor);//上方向
}

void CPlayer::PostRender() {
	if (!m_isDrawHUD)return;

	CVector3 tdFrontPos = m_cam.CalcScreenPosFromWorldPos(m_cam.GetPos() + m_hotoke.GetFront()*380.0f);

	//速度
	if (tdFrontPos.z > 0.0f && tdFrontPos.z < 1.0f) {
		float kmh;
		//時速
		kmh = m_hotoke.GetMove().Length()*GetEngine().GetStandardFrameRate()*60.0f*60.0f / METER / 1000.0f;
		m_HUDFont.DrawFormat(L"%.1f", tdFrontPos - CVector3(0.06f,  0.025f, 0.0f), { 1.0f,1.0f }, kmh);
		//落下速度
		kmh = -m_hotoke.GetMove().y*GetEngine().GetStandardFrameRate()*60.0f*60.0f / METER / 1000.0f;
		if (abs(kmh) < 0.1f) { kmh = 0.0f; }
		m_HUDFont.DrawFormat(L"%.1f", tdFrontPos - CVector3(0.06f, 0.0f, 0.0f), { 1.0f,0.0f }, kmh);
	}
	//目標の名前・距離・接近速度・接触秒数

	CVector3 pos;

	//ガンクロス(照準)
	if (m_isLockon) {
		pos = m_cam.CalcScreenPosFromWorldPos(m_hotoke.GetTargetPos());
		if (pos.z > 0.0f && pos.z < 1.0f) { m_guncross.Draw(pos, 1.0f, 0.5f, 0.0f, m_HUDColor); }// CVector4(1.0f, 0.0f, 1.0f, 0.75f)); }
	}
	else {
		/*CVector3 resultPos[2];
		int result = CMath::IntersectLines(resultPos, m_hotoke.GetPos(), m_hotoke.GetTargetPos(), m_cam.GetPos(), m_cam.GetVanishingPoint());
		if (result == 1) {
			CVector3 point = (resultPos[0] + resultPos[1]) / 2.0f;
			pos = m_cam.CalcScreenPosFromWorldPos(point);
			if (pos.z > 0.0f && pos.z < 1.0f) { m_guncross.Draw(pos, 1.0f, 0.5f, 0.0f, m_HUDColor); }
		}*/
		pos = m_cam.CalcScreenPosFromWorldPos(m_hotoke.GetPos() + (m_hotoke.GetTargetPos()- m_hotoke.GetPos())*0.125f*0.5f);
		if (pos.z > 0.0f && pos.z < 1.0f) { m_guncross.Draw(pos, 1.0f, 0.5f, CMath::PI_QUARTER, m_HUDColor); }
	}
	
	//ウイスキーマーク(機体の向き)
	pos = tdFrontPos;
	if (pos.z > 0.0f && pos.z < 1.0f) { m_wMark.Draw(pos, 1.0f, 0.5f, 0.0f, m_HUDColor); }

	//ベロシティベクトル(進行方向)
	if (m_hotoke.GetMove().LengthSq() > 1.0f) {
		pos = m_cam.CalcScreenPosFromWorldPos(m_hotoke.GetPos() + m_hotoke.GetMove().GetNorm() * (m_cam.GetFar()*0.5f));
		if (pos.z > 0.0f && pos.z < 1.0f) { m_velocityVector.Draw(pos, 0.75f, 0.5f, 0.0f, m_HUDColor); }
	}

	//バックミラー
	if (m_cam.GetIsBackMirror()) {
		m_HUDFont.Draw(L"((BACK VIEW))", { 0.5f,0.95f }, { 0.5f,0.0f });
	}
}
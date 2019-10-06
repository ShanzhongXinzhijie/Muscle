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
	m_human.SetPos(CVector3::AxisY()*900.0f + CVector3::AxisX()*50.0f + CVector3::AxisZ()*100.0f);
	m_human.SetScale(10.0f);

	//m_humanCam.SetViewAngleDeg(25.0f);
	m_humanCam.SetFar(15000.0f);

	m_hotoke.SetBodyPart(CDeathHotoke::enHead, std::make_unique<BP_FishHead>(&m_hotoke));
	m_hotoke.SetBodyPart(CDeathHotoke::enArm, std::make_unique<BP_KaniArm>(&m_hotoke));
	m_hotoke.SetBodyPart(CDeathHotoke::enWing, std::make_unique<BP_BirdWing>(&m_hotoke));
	m_hotoke.SetBodyPart(CDeathHotoke::enLeg, std::make_unique<BP_HumanLeg>(&m_hotoke));

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
			constexpr int screenSize3D = 320;
			constexpr int HUDSize = 100;
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
	bool isLock = false; float distance = 0.0f; CVector3 outPos;
	QueryGOs<IFu>(L"LockableObject", [&](IFu* go) {
		if (&m_hotoke == go) { return true; }//自分は除く

		CVector3 screenPos = m_cam.CalcScreenPosFromWorldPos(go->GetCollisionPos());
		//位置が画面内か?
		if (screenPos.x > 0.0f && screenPos.x < 1.0f && screenPos.y > 0.0f && screenPos.y < 1.0f && screenPos.z > 0.0f && screenPos.z < 1.0f) {
			if (!isLock || distance > screenPos.LengthSq()) {
				isLock = true;
				outPos = go->GetCollisionPos();
				distance = screenPos.LengthSq();
			}
		}
		return true; 
	});
	//ホトケのターゲット位置設定
	if (isLock) {
		m_hotoke.SetTargetPos(outPos);
	}
	else {
		m_hotoke.SetTargetPos(m_cam.GetVanishingPoint());
	}
}
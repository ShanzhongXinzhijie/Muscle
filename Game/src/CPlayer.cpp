#include "stdafx.h"
#include "CPlayer.h"
#include "BP_FishHead.h"

bool CPlayer::Start() {	
	m_anim.Load(L"Resource/animation/human/stand.tka");
	m_human.Init(L"Resource/modelData/human.cmo", &m_anim, 1);
	m_human.SetPos(CVector3::AxisY()*900.0f + CVector3::AxisX()*50.0f + CVector3::AxisZ()*100.0f);
	m_human.SetScale(10.0f);

	//m_humanCam.SetViewAngleDeg(25.0f);
	m_humanCam.SetFar(15000.0f);

	m_hotoke.SetBodyPart(CDeathHotoke::enHead, std::make_unique<BP_FishHead>(&m_hotoke));

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
	if (GetKeyInput('F')) {
		m_humanCam.SetPos(m_human.GetBonePos(m_human.FindBoneID(L"Head"))+CVector3::AxisY()*0);
		m_humanCam.SetTarget(m_hotoke.GetPos());
		SetMainCamera(&m_humanCam);
	}
	else {
		m_cam.SetToMainCamera();
	}

	//ホトケのターゲット位置設定
	m_hotoke.SetTargetPos(m_cam.GetVanishingPoint());
}
#include "stdafx.h"
#include "CPlayer.h"


CPlayer::CPlayer()
{
}


CPlayer::~CPlayer()
{
}

bool CPlayer::Start() {
	m_cam.SetPos(CVector3::AxisY()*900.0f);
	
	//m_human.Init(L"Resource/modelData/human.cmo", enFbxUpAxisY);
	m_human.SetPos(CVector3::AxisY()*800.0f);

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
		pos.y += 2.0f;
	}
	if (GetKeyInput('C')) {
		pos.y -= 2.0f;
	}
	m_hotoke.SetPos(pos);

	//m_cam.SetPos(m_hotoke.GetPos() + CVector3(100.0f, -50.0f, -500.0f));
	m_cam.SetTarget(m_hotoke.GetPos() + CVector3(100.0f, 0.0f, 0.0f));
}
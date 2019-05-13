#include "stdafx.h"
#include "CDeathHotoke.h"


CDeathHotoke::CDeathHotoke()
{
}


CDeathHotoke::~CDeathHotoke()
{
}

bool CDeathHotoke::Start() {
	m_model[0].Init(L"Resource/modelData/core.cmo");
	m_model[1].Init(L"Resource/modelData/humanleg.cmo");
	m_model[2].Init(L"Resource/modelData/fishhead.cmo");
	m_model[3].Init(L"Resource/modelData/birdwing.cmo");
	m_model[4].Init(L"Resource/modelData/kaniarm.cmo");
	for (auto& model : m_model) {
		model.SetScale(CVector3::One()*0.01f);
	}
	m_pos = CVector3::AxisY()*100.0f;
	return true;
}

void CDeathHotoke::Update() {

	CVector3 moveDir;
	if (GetKeyInput('W')) {
		moveDir += m_cam.GetFront();
	}
	if (GetKeyInput('S')) {
		moveDir += m_cam.GetFront()*-1.0f;
	}
	if (GetKeyInput('A')) {
		moveDir += m_cam.GetLeft();
	}
	if (GetKeyInput('D')) {
		moveDir += m_cam.GetLeft()*-1.0f;
	}
	moveDir.Normalize();
	m_pos += moveDir * 10.0f;

	for (auto& model : m_model) {
		model.SetPos(m_pos);
	}
	m_cam.SetPos(m_pos);
}
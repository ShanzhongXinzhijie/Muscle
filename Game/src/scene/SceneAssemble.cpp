#include "stdafx.h"
#include "SceneAssemble.h"
#include "CDeathHotoke.h"

bool SceneAssemble::Start() {
	//	m_hotoke = std::make_unique<CDeathHotoke>();
	

	return true;
}

void SceneAssemble::Update() {

}

void SceneAssemble::PostRender() {
	m_font.Draw(L"���݂̃p�[�c", {});
	m_font.Draw(L"��D", {});
}


bool SceneMulligan::Start() {
	return true;
}

void SceneMulligan::Update() {
	//�I���ł���p�[�c�����D�I��
	//
}

void SceneMulligan::PostRender() {
	//HUD�ɕ`��
}
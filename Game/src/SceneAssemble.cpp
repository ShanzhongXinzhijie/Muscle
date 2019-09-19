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
	m_font.Draw(L"現在のパーツ", {});
	m_font.Draw(L"手札", {});
}


bool SceneMulligan::Start() {
	return true;
}

void SceneMulligan::Update() {
	//選択できるパーツから手札選ぶ
	//
}

void SceneMulligan::PostRender() {
	//HUDに描画
}
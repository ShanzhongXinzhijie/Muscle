#include "stdafx.h"
#include "SceneAssemble.h"
#include "CDeathHotoke.h"

bool SceneAssemble::Start() {
	for (int i = 0; i < m_hotokeNum; i++) {
	//	m_hotoke[i] = std::make_unique<CDeathHotoke>();
	}

	return true;
}

void SceneAssemble::Update() {

}

void SceneAssemble::PostRender() {
	m_font.Draw(L"現在のパーツ", {});
	m_font.Draw(L"手札", {});
}
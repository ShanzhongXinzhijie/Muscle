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
	m_font.Draw(L"Œ»İ‚Ìƒp[ƒc", {});
	m_font.Draw(L"èD", {});
}
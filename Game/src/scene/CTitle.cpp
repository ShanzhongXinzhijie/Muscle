#include "stdafx.h"
#include "CTitle.h"
#include "WindowSizeManager.h"
#include "LoadingScreen.h"
#include "AssembleScene.h"

bool CTitle::Start() {
	//画面サイズ変更
	FindGO<WindowSizeManager>(L"WindowSizeManager")->ChangeWindowSize(true);

	m_sprite.Init(L"Resource/spriteData/logo.png");
	return true;
}

void CTitle::Update() {
	for (int i = 0; i < PLAYER_NUM; i++) {
		if (!m_enter) {
			//タイトル
			//PRESS ANY BUTTON
			if (Pad(i).GetAnyDown()) {
				m_enter = true;
				break;
			}
		}
		else {
			//モード選択
			if (Pad(i).GetDown(enButtonLeft) || Pad(i).GetDown(enButtonLSLeft)) {
				m_selectMode--; if (m_selectMode < 0) { m_selectMode = 0; }
			}
			if (Pad(i).GetDown(enButtonRight) || Pad(i).GetDown(enButtonLSRight)) {
				m_selectMode++; if (m_selectMode > 1) { m_selectMode = 1; }
			}
			//決定
			if (Pad(i).GetDown(enButtonA) || Pad(i).GetDown(enButtonStart)) {
				//画面サイズ変更
				FindGO<WindowSizeManager>(L"WindowSizeManager")->ChangeWindowSize(m_selectMode == 0);
				//遷移
				if (m_selectMode == 0) {
					new LoadingScreen([]() {new AssembleScene(2); });
				}
				else {
					new LoadingScreen([]() {new AssembleScene(1); });
				}
				delete this;
				return;
			}
		}
	}
}

void CTitle::PostLoopUpdate() {
	//背景
	DrawQuad2D(0.0f, 1.0f, CVector4::White());
}

void CTitle::PostRender() {
	//ロゴ
	m_sprite.Draw({ 0.5f,0.5f }, CVector2::One(), { 0.5f,0.5f });

	if(!m_enter) {
		m_font.Draw(L"ｭｭｭｭｭデスホトケ", { 0.5f,0.0f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });
		m_font.Draw(L"PRESS ANY BUTTON ->THE ORDER", { 0.5f,0.85f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
		return;
	}

	//タイトル
	m_font.Draw(L"モードをせんたく", { 0.5f,0.0f }, CVector4::Black(), CVector2::One(), { 0.5f,0.0f });

	//モードセレクト
	for (int i = 0; i < 2; i++) {
		CVector4 color = CVector4::Black();
		if (i == m_selectMode) {
			color = CVector4::Red();
		}
		if (i == 0) {
			m_font.Draw(L"ひだり VS みぎ", { 0.25f,0.85f }, color, CVector2::One(), { 0.0f,0.5f });
		}
		else {
			m_font.Draw(L"おまえ VS メカ", { 0.5f,0.85f }, color, CVector2::One(), { 0.0f,0.5f });
		}
	}

	//モード説明
	if (0 == m_selectMode) {
		m_font.Draw(L"ともだちとたたかいます", { 0.5f,0.95f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
	}
	else {
		m_font.Draw(L"きかいのともだちとたたかいます", { 0.5f,0.95f }, CVector4::Black(), CVector2::One(), { 0.5f,0.5f });
	}
}
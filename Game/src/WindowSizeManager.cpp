#include "stdafx.h"
#include "WindowSizeManager.h"

namespace {
	//��ʃT�C�Y�ݒ�
	constexpr int screenSize = SCREEN_SIZE_2D;
	constexpr int HUDSize = SCREEN_SPRITE_BAR;
}

bool WindowSizeManager::Start() {
	SetName(L"WindowSizeManager");
	return true;
}

void WindowSizeManager::ChangeWindowSize(bool isTwoScreen) {
	//��ʃT�C�Y�ύX
	if (!isTwoScreen) {
		//1���
		GetEngine().ChangeWindowSize(m_windowSize, m_windowSize);
		GetGraphicsEngine().ChangeFrameBufferSize(
			screenSize, screenSize,
			m_3dScreenSize, m_3dScreenSize,
			screenSize, screenSize,
			enNoSplit
		);
	}
	else {
		//2���
		GetEngine().ChangeWindowSize(m_windowSize * 2 + HUDSize, m_windowSize);
		float screensSpliteSize[8] = {
			0.0f,0.0f,(float)screenSize / (screenSize * 2 + HUDSize),1.0f,
			1.0f - (float)screenSize / (screenSize * 2 + HUDSize),0.0f,1.0f,1.0f,
		};
		GetGraphicsEngine().ChangeFrameBufferSize(
			screenSize * 2 + HUDSize, screenSize,
			m_3dScreenSize, m_3dScreenSize,
			screenSize, screenSize,
			enSide_TwoSplit,
			screensSpliteSize
		);
	}
}

//void WindowSizeManager::Update() {
//	for (int i = 0; i < PLAYER_NUM; i++) {
//		//�ݒ���ON�EOFF
//		if (Pad(i).GetDown(enButtonBack)) {
//			m_isEnable = !m_isEnable;
//		}
//	}
//}
//
//void WindowSizeManager::PostLoopUpdate() {
//	if (!m_isEnable) { return; }
//	//�w�i
//	DrawQuad2D(0.25f, 1.0f-0.25f, CVector4::Black()*0.5f);
//}
//
//void WindowSizeManager::PostRender() {
//	if (!m_isEnable) { return; }
//
//}
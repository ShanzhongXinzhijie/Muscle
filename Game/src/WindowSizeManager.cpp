#include "stdafx.h"
#include "WindowSizeManager.h"

namespace {
	//画面サイズ設定
	constexpr int HUDSize = SCREEN_SIZE_2D;
	constexpr int SpriteBarSize = SCREEN_SPRITE_BAR;
}

bool WindowSizeManager::Start() {
	SetName(L"WindowSizeManager");
	return true;
}

void WindowSizeManager::ChangeWindowSize(bool isTwoScreen) {
	//画面サイズ変更
	if (!isTwoScreen) {
		//1画面
		GetEngine().ChangeWindowSize(m_windowSize, m_windowSize);//ウィンドウサイズ
		//フレームバッファサイズ
		GetGraphicsEngine().ChangeFrameBufferSize(
			m_screenSize, m_screenSize,//画面サイズ
			m_3dScreenSize, m_3dScreenSize,//3d解像度
			HUDSize, HUDSize,//hud解像度
			enNoSplit//分割じゃない
		);
	}
	else {
		//2画面
		GetEngine().ChangeWindowSize(m_windowSize * 2 + SpriteBarSize, m_windowSize);//ウィンドウサイズ
		//画面分割比率
		float screensSpliteSize[8] = {
			0.0f,0.0f,(float)m_screenSize / (m_screenSize * 2 + SpriteBarSize),1.0f,
			1.0f - (float)m_screenSize / (m_screenSize * 2 + SpriteBarSize),0.0f,1.0f,1.0f,
		};
		//フレームバッファサイズ
		GetGraphicsEngine().ChangeFrameBufferSize(
			m_screenSize * 2 + SpriteBarSize, m_screenSize,//画面サイズ
			m_3dScreenSize, m_3dScreenSize,//3d解像度
			HUDSize, HUDSize,//hud解像度
			enSide_TwoSplit,//画面分割
			screensSpliteSize//画面分割比率
		);
	}
}
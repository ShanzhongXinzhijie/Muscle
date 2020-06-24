#include "stdafx.h"
#include "WindowSizeManager.h"

namespace {
	//��ʃT�C�Y�ݒ�
	constexpr int HUDSize = SCREEN_SIZE_2D;
	constexpr int SpriteBarSize = SCREEN_SPRITE_BAR;
}

bool WindowSizeManager::Start() {
	SetName(L"WindowSizeManager");
	return true;
}

void WindowSizeManager::ChangeWindowSize(bool isTwoScreen) {
	//��ʃT�C�Y�ύX
	if (!isTwoScreen) {
		//1���
		GetEngine().ChangeWindowSize(m_windowSize, m_windowSize);//�E�B���h�E�T�C�Y
		//�t���[���o�b�t�@�T�C�Y
		GetGraphicsEngine().ChangeFrameBufferSize(
			m_screenSize, m_screenSize,//��ʃT�C�Y
			m_3dScreenSize, m_3dScreenSize,//3d�𑜓x
			HUDSize, HUDSize,//hud�𑜓x
			enNoSplit//��������Ȃ�
		);
	}
	else {
		//2���
		GetEngine().ChangeWindowSize(m_windowSize * 2 + SpriteBarSize, m_windowSize);//�E�B���h�E�T�C�Y
		//��ʕ����䗦
		float screensSpliteSize[8] = {
			0.0f,0.0f,(float)m_screenSize / (m_screenSize * 2 + SpriteBarSize),1.0f,
			1.0f - (float)m_screenSize / (m_screenSize * 2 + SpriteBarSize),0.0f,1.0f,1.0f,
		};
		//�t���[���o�b�t�@�T�C�Y
		GetGraphicsEngine().ChangeFrameBufferSize(
			m_screenSize * 2 + SpriteBarSize, m_screenSize,//��ʃT�C�Y
			m_3dScreenSize, m_3dScreenSize,//3d�𑜓x
			HUDSize, HUDSize,//hud�𑜓x
			enSide_TwoSplit,//��ʕ���
			screensSpliteSize//��ʕ����䗦
		);
	}
}
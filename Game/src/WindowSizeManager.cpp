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
		GetEngine().ChangeWindowSize(m_windowSize, m_windowSize);//�E�B���h�E�T�C�Y
		//�t���[���o�b�t�@�T�C�Y
		GetGraphicsEngine().ChangeFrameBufferSize(
			screenSize, screenSize,//��ʃT�C�Y
			m_3dScreenSize, m_3dScreenSize,//3d�𑜓x
			screenSize, screenSize,//hud�𑜓x
			enNoSplit//��������Ȃ�
		);
	}
	else {
		//2���
		GetEngine().ChangeWindowSize(m_windowSize * 2 + HUDSize, m_windowSize);//�E�B���h�E�T�C�Y
		//��ʕ����䗦
		float screensSpliteSize[8] = {
			0.0f,0.0f,(float)screenSize / (screenSize * 2 + HUDSize),1.0f,
			1.0f - (float)screenSize / (screenSize * 2 + HUDSize),0.0f,1.0f,1.0f,
		};
		//�t���[���o�b�t�@�T�C�Y
		GetGraphicsEngine().ChangeFrameBufferSize(
			screenSize * 2 + HUDSize, screenSize,//��ʃT�C�Y
			m_3dScreenSize, m_3dScreenSize,//3d�𑜓x
			screenSize, screenSize,//hud�𑜓x
			enSide_TwoSplit,//��ʕ���
			screensSpliteSize//��ʕ����䗦
		);
	}
}
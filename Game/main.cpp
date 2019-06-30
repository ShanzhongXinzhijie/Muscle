#include "stdafx.h"
#include "Game.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//�Q�[���̏������B
	InitEngineParameter initparam;
	
	initparam.windowStyle = WS_POPUP;
	initparam.screenWidth = 720;			//�E�B���h�E�̕�
	initparam.screenHeight = 720;			//�E�B���h�E�̍���
	initparam.frameBufferWidth = 720;		//�t���[���o�b�t�@�̕��B���ꂪ�����𑜓x�B
	initparam.frameBufferHeight = 720;		//�t���[���o�b�t�@�̍����B���ꂪ�����𑜓x�B
	initparam.frameBufferWidth3D = 724;		//3D�`��̉𑜓x(��)
	initparam.frameBufferHeight3D = 724;	//3D�`��̉𑜓x(����)

	//initparam.isEnableDOF = true;
	//initparam.standardFps = 25;
	//initparam.limitFps = 25;
	//initparam.shadowMapSetting = enON;
	
	GetEngine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "���ɍ����̃f�X�z�g�P", initparam);	

	//DOF�ݒ�
	GetGraphicsEngine().GetDOFRender().SetFocus(150.0f);
	GetGraphicsEngine().GetDOFRender().SetFocusArea(50.0f);
	GetGraphicsEngine().GetDOFRender().SetNear(50.0f);
	GetGraphicsEngine().GetDOFRender().SetFar(250.0f);
	
	//����\��
	//SetPhysicsDebugDrawMode(btIDebugDraw::DBG_DrawWireframe);

	SetIsDebugDraw(true);
	SetIsDebugInput(true);

	Game game;

	//�Q�[�����[�v�B
	GetEngine().RunGameLoop();
}
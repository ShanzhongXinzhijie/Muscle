#include "stdafx.h"
#include "Game.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//�Q�[���̏������B
	InitEngineParameter initparam;
	initparam.isEnableDOF = true;
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
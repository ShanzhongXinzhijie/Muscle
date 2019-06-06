#include "stdafx.h"
#include "Game.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	InitEngineParameter initparam;
	initparam.isEnableDOF = true;
	//initparam.standardFps = 25;
	//initparam.limitFps = 25;
	//initparam.shadowMapSetting = enON;
	GetEngine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "究極混成体デスホトケ", initparam);	

	//DOF設定
	GetGraphicsEngine().GetDOFRender().SetFocus(150.0f);
	GetGraphicsEngine().GetDOFRender().SetFocusArea(50.0f);
	GetGraphicsEngine().GetDOFRender().SetNear(50.0f);
	GetGraphicsEngine().GetDOFRender().SetFar(250.0f);
	
	//判定表示
	//SetPhysicsDebugDrawMode(btIDebugDraw::DBG_DrawWireframe);

	SetIsDebugDraw(true);
	SetIsDebugInput(true);

	Game game;

	//ゲームループ。
	GetEngine().RunGameLoop();
}
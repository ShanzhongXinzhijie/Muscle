#include "stdafx.h"
#include "Game.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//エンジンの初期化設定
	InitEngineParameter initparam;
	
	initparam.windowStyle = WS_POPUP;		//ウィンドウスタイル
	initparam.screenWidth = 720;			//ウィンドウの幅
	initparam.screenHeight = 720;			//ウィンドウの高さ
	initparam.frameBufferWidth = 720;		//フレームバッファの幅。これが内部解像度。
	initparam.frameBufferHeight = 720;		//フレームバッファの高さ。これが内部解像度。
	initparam.frameBufferWidth3D = 724;		//3D描画の解像度(幅)
	initparam.frameBufferHeight3D = 724;	//3D描画の解像度(高さ)

	//initparam.isEnableDOF = true;
	//initparam.standardFps = 25;
	//initparam.limitFps = 25;
	//initparam.shadowMapSetting = enON;
	
	//エンジン初期化
	GetEngine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "究極混成体デスホトケ", initparam);	

	//DOF設定
	GetGraphicsEngine().GetDOFRender().SetFocus(150.0f);
	GetGraphicsEngine().GetDOFRender().SetFocusArea(50.0f);
	GetGraphicsEngine().GetDOFRender().SetNear(50.0f);
	GetGraphicsEngine().GetDOFRender().SetFar(250.0f);
	
	//判定表示
	//SetPhysicsDebugDrawMode(btIDebugDraw::DBG_DrawWireframe);

	//デバッグ表示・入力、常時有効化
	SetIsDebugDraw(true);
	SetIsDebugInput(true);

	//ゲームインスタンス作成
	Game game;

	//ゲームループ。
	GetEngine().RunGameLoop();
}
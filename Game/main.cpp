#include "stdafx.h"
#include "LoadingScreen.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	//エンジンの初期化設定
	InitEngineParameter initparam;
	
	//TODO フルスク対応
	initparam.windowStyle = WS_POPUP | WS_BORDER ;		//ウィンドウスタイル
	initparam.screenWidth = 640;			//ウィンドウの幅
	initparam.screenHeight = 640;			//ウィンドウの高さ
	initparam.frameBufferWidth = 640;		//フレームバッファの幅。これが内部解像度。
	initparam.frameBufferHeight = 640;		//フレームバッファの高さ。これが内部解像度。
	
	initparam.frameBufferWidth3D = 1080;	//3D描画の解像度(幅)
	initparam.frameBufferHeight3D = 1080;	//3D描画の解像度(高さ)
	//initparam.isAntiAliasing = false;		//アンチエイリアス無効

	initparam.SDUnityChanScale = 3.5f;		//距離スケール設定
	//0.15f

	initparam.isEnableDOF = true;
	//initparam.standardFps = 25;
	//initparam.limitFps = 25;
	//initparam.shadowMapSetting = enON;

	//TODO:リリース版では消すこと
	//{
	//	char str[256] = "";
	//	DWORD len = sizeof(str);
	//	GetComputerName(str, &len);
	//	if(strcmp("LAPTOP-TNVKUPNN", str)==0){
	//		GetUserName(str, &len);
	//		if (strcmp("GC1828", str) == 0) {
	//			//低スペック設定
	//			initparam.frameBufferWidth3D = 640;		//3D描画の解像度(幅)
	//			initparam.frameBufferHeight3D = 640;	//3D描画の解像度(高さ)
	//		}
	//	}
	//}
	
	//エンジン初期化
	GetEngine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "究極混成体デスホトケ", initparam);	

	//DOF設定
	GetGraphicsEngine().GetDOFRender().SetFocus(0.0f);
	GetGraphicsEngine().GetDOFRender().SetFocusArea(0.0f);
	GetGraphicsEngine().GetDOFRender().SetNear(0.0f);
	GetGraphicsEngine().GetDOFRender().SetFar(30000.0f);

	//歪曲収差設定
	SetLensDistortionScale(5.0f);
	
	//判定表示
	//SetPhysicsDebugDrawMode(btIDebugDraw::DBG_DrawWireframe);

	//デバッグ表示・入力、常時有効化
	SetIsDebugDraw(true);
	SetIsDebugInput(true);

	//シェーダー
	//※デバッグ用
	ShaderResources::GetInstance().SetIsReplaceForEngineFilePath(true);

	//フォント設定
	CFont::LoadDefaultFont(L"Resource/font/x14y24pxHeadUpDaisy.spritefont");//eunomia_0200/Eunomia.spritefont");
	
	//画面分割
	constexpr int screenSize = 640;
	constexpr int screenSize3D = 640;
	constexpr int HUDSize = 64;
	GetEngine().ChangeWindowSize(screenSize * 2 + HUDSize, screenSize);
	float screensSize[8] = {
		0.0f,0.0f,(float)screenSize / (screenSize * 2 + HUDSize),1.0f,
		1.0f - (float)screenSize / (screenSize * 2 + HUDSize),0.0f,1.0f,1.0f,
	};
	GetGraphicsEngine().ChangeFrameBufferSize(screenSize * 2 + HUDSize, screenSize, screenSize3D * 2 + HUDSize, screenSize3D, enSide_TwoSplit, screensSize);
		
	//ゲームインスタンス作成
	LoadingScreen game;

	//ゲームループ。
	GetEngine().RunGameLoop();
}
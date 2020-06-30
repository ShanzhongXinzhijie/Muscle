#include "stdafx.h"
#include "CTitle.h"
#include "LoadingScreen.h"
#include "WindowSizeManager.h"
#include "assemblescene.h"
#include "CGameMode.h"
#include "ImposterViewer.h"
#include "CConfig.h"

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
	
	initparam.frameBufferWidth3D = 640;// 1080;	//3D描画の解像度(幅)
	initparam.frameBufferHeight3D = 640;//1080;	//3D描画の解像度(高さ)
	//initparam.isAntiAliasing = false;		//アンチエイリアス無効

	initparam.SDUnityChanScale = 3.5f;		//距離スケール設定
	//0.15f

	//DOF設定
	initparam.isEnableDOF = true;
	//initparam.DOFBufferScale = 0.3f;

	//initparam.standardFps = 25;
	//initparam.limitFps = 25;
	//initparam.shadowMapSetting = enON;

	/*initparam.isEnableBloom = false;
	initparam.isEnableSSAO = false;
	initparam.isEnableDOF = false;*/
	//initparam.SSAOBufferScale = 1.0f;

	//シェーダー
	//※デバッグ用
	initparam.isShaderPathReplaceForEngineFilePath = true;
	//更新のあるシェーダーを再コンパイルするか
	//※リリース時にコメント外す
	//initparam.isShaderRecompile = false;

	//エンジン初期化
	GetEngine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "究極混成体デスホトケ", initparam);	

	//DOF設定
	GetGraphicsEngine().GetDOFRender().SetFocus(0.0f);
	GetGraphicsEngine().GetDOFRender().SetFocusArea(10000.0f);
	GetGraphicsEngine().GetDOFRender().SetNear(0.0f);
	GetGraphicsEngine().GetDOFRender().SetFar(30000.0f);

	//歪曲収差設定
	SetLensDistortionScale(5.0f);

	//物理ワールド設定
	GetPhysicsWorld().SetGravity({ 0.0f,-800.0f,0.0f });//重力
	//GetPhysicsWorld().GetSoftBodyWorldInfo()->air_density = 1.2f*10.0f;
	
	//判定表示
	//SetPhysicsDebugDrawMode(btIDebugDraw::DBG_DrawWireframe);

	//デバッグ表示・入力、常時有効化
	//SetIsDebugDraw(true);
	//SetIsDebugInput(true);

	//フォント設定
	CFont::LoadDefaultFont(L"Resource/font/x14y24pxHeadUpDaisy.spritefont");

	//セーブデータを読み込む
	CConfig config;
	config.Load();
	
	//画面分割マネージャー
	WindowSizeManager windowSizeManager(config.GetConfigData().windowScale);
	windowSizeManager.ChangeWindowSize(false);

	//デスホトケアセンブルマネージャー
	HotokeAssembleManager hotokeAssembleManager;

	//ゲームモード
	CGameMode gameMode;
	//SE
	GameSE::SetGameModePtr(&gameMode);
	//SE全体のボリュームを設定
	SuicideObj::CSE::GetSubmixVoice()->SetVolume(0.5f);
	//BGM
	BGMPlayer bgmPlayer;
	//BGM全体のボリュームを設定
	SuicideObj::CBGM::GetSubmixVoice()->SetVolume(0.5f);

	//事前ロード
	{
		GameObj::CSkinModelRender m_model;
		m_model.Init(L"Resource/modelData/unchi.cmo");
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
		TextureFactory::GetInstance().Load(L"Resource/normalMap/unchi_n.jpg", nullptr, &textureView);
	}

	//ゲームインスタンス作成
	new LoadingScreen([]() {new CTitle; });
	//new LoadingScreen([]() {new ImposterViewer; });

	//ゲームループ。
	GetEngine().RunGameLoop();

	//セーブ
	config.GetConfigData().windowScale = windowSizeManager.GetWindowScale();
	config.Save();
}
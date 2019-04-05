#include "stdafx.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	InitEngineParameter initparam;
	GetEngine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Game", initparam);	
	
	//ライト作成
	GameObj::CDirectionLight l;
	l.SetDirection(CVector3::Down());
	l.SetColor(CVector3::One()*0.5f);

	//カメラ作成
	GameObj::PerspectiveCamera cam;
	SetMainCamera(&cam);
	cam.SetPos(CVector3::Up()*200.0f + CVector3::AxisZ()*-400.0f);

	//ゲームループ。
	GetEngine().RunGameLoop();

}
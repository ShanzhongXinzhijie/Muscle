#include "stdafx.h"
#include "CTitle.h"
#include "LoadingScreen.h"
#include "WindowSizeManager.h"
#include "assemblescene.h"
#include "CGameMode.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

	//�G���W���̏������ݒ�
	InitEngineParameter initparam;
	
	//TODO �t���X�N�Ή�
	initparam.windowStyle = WS_POPUP | WS_BORDER ;		//�E�B���h�E�X�^�C��
	initparam.screenWidth = 640;			//�E�B���h�E�̕�
	initparam.screenHeight = 640;			//�E�B���h�E�̍���
	initparam.frameBufferWidth = 640;		//�t���[���o�b�t�@�̕��B���ꂪ�����𑜓x�B
	initparam.frameBufferHeight = 640;		//�t���[���o�b�t�@�̍����B���ꂪ�����𑜓x�B
	
	initparam.frameBufferWidth3D = 640;// 1080;	//3D�`��̉𑜓x(��)
	initparam.frameBufferHeight3D = 640;//1080;	//3D�`��̉𑜓x(����)
	//initparam.isAntiAliasing = false;		//�A���`�G�C���A�X����

	initparam.SDUnityChanScale = 3.5f;		//�����X�P�[���ݒ�
	//0.15f

	initparam.isEnableDOF = true;
	//initparam.standardFps = 25;
	//initparam.limitFps = 25;
	//initparam.shadowMapSetting = enON;

	//�V�F�[�_�[
	//���f�o�b�O�p
	initparam.isShaderPathReplaceForEngineFilePath = true;
	//initparam.SSAOBufferScale = 1.0f;

	//TODO:�����[�X�łł͏�������
	//{
	//	char str[256] = "";
	//	DWORD len = sizeof(str);
	//	GetComputerName(str, &len);
	//	if(strcmp("LAPTOP-TNVKUPNN", str)==0){
	//		GetUserName(str, &len);
	//		if (strcmp("GC1828", str) == 0) {
	//			//��X�y�b�N�ݒ�
	//			initparam.frameBufferWidth3D = 640;		//3D�`��̉𑜓x(��)
	//			initparam.frameBufferHeight3D = 640;	//3D�`��̉𑜓x(����)
	//		}
	//	}
	//}
	
	//�G���W��������
	GetEngine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "���ɍ����̃f�X�z�g�P", initparam);	

	//DOF�ݒ�
	GetGraphicsEngine().GetDOFRender().SetFocus(0.0f);
	GetGraphicsEngine().GetDOFRender().SetFocusArea(0.0f);
	GetGraphicsEngine().GetDOFRender().SetNear(0.0f);
	GetGraphicsEngine().GetDOFRender().SetFar(30000.0f);

	//�c�Ȏ����ݒ�
	SetLensDistortionScale(5.0f);
	
	//����\��
	//SetPhysicsDebugDrawMode(btIDebugDraw::DBG_DrawWireframe);

	//�f�o�b�O�\���E���́A�펞�L����
	SetIsDebugDraw(true);
	SetIsDebugInput(true);

	//�t�H���g�ݒ�
	CFont::LoadDefaultFont(L"Resource/font/x14y24pxHeadUpDaisy.spritefont");
	
	//��ʕ����}�l�[�W���[
	WindowSizeManager windowSizeManager;
	windowSizeManager.ChangeWindowSize(true);//���ʂɂ���

	//�f�X�z�g�P�A�Z���u���}�l�[�W���[
	HotokeAssembleManager hotokeAssembleManager;

	//�Q�[�����[�h
	CGameMode gameMode;

	//�Q�[���C���X�^���X�쐬
	new LoadingScreen([]() {new CTitle; });

	//�Q�[�����[�v�B
	GetEngine().RunGameLoop();
}
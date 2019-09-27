#include "stdafx.h"
#include "LoadingScreen.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//�G���W���̏������ݒ�
	InitEngineParameter initparam;
	
	//TODO �t���X�N�Ή�
	initparam.windowStyle = WS_POPUP | WS_BORDER ;		//�E�B���h�E�X�^�C��
	initparam.screenWidth = 640;			//�E�B���h�E�̕�
	initparam.screenHeight = 640;			//�E�B���h�E�̍���
	initparam.frameBufferWidth = 640;		//�t���[���o�b�t�@�̕��B���ꂪ�����𑜓x�B
	initparam.frameBufferHeight = 640;		//�t���[���o�b�t�@�̍����B���ꂪ�����𑜓x�B
	
	initparam.frameBufferWidth3D = 1080;	//3D�`��̉𑜓x(��)
	initparam.frameBufferHeight3D = 1080;	//3D�`��̉𑜓x(����)
	//initparam.isAntiAliasing = false;		//�A���`�G�C���A�X����

	initparam.SDUnityChanScale = 3.5f;		//�����X�P�[���ݒ�
	//0.15f

	initparam.isEnableDOF = true;
	//initparam.standardFps = 25;
	//initparam.limitFps = 25;
	//initparam.shadowMapSetting = enON;

	initparam.isSplitScreen = enSide_TwoSplit;

	//TODO:�����[�X�łł͏�������
	{
		char str[256] = "";
		DWORD len = sizeof(str);
		GetComputerName(str, &len);
		if(strcmp("LAPTOP-TNVKUPNN", str)==0){
			GetUserName(str, &len);
			if (strcmp("GC1828", str) == 0) {
				//��X�y�b�N�ݒ�
				initparam.frameBufferWidth3D = 320;		//3D�`��̉𑜓x(��)
				initparam.frameBufferHeight3D = 320;	//3D�`��̉𑜓x(����)
			}
		}
	}
	
	//�G���W��������
	GetEngine().InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "���ɍ����̃f�X�z�g�P", initparam);	

	//DOF�ݒ�
	GetGraphicsEngine().GetDOFRender().SetFocus(0.0f);
	GetGraphicsEngine().GetDOFRender().SetFocusArea(0.0f);
	GetGraphicsEngine().GetDOFRender().SetNear(0.0f);
	GetGraphicsEngine().GetDOFRender().SetFar(30000.0f);
	
	//����\��
	//SetPhysicsDebugDrawMode(btIDebugDraw::DBG_DrawWireframe);

	//�f�o�b�O�\���E���́A�펞�L����
	SetIsDebugDraw(true);
	SetIsDebugInput(true);

	//�Q�[���C���X�^���X�쐬
	LoadingScreen game;

	//�Q�[�����[�v�B
	GetEngine().RunGameLoop();
}
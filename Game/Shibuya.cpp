#include "stdafx.h"
#include "Shibuya.h"


Shibuya::Shibuya()
{
	//�X���f��
	m_model.Init(L"Resource/modelData/city.cmo");
	m_model.GetSkinModel().FindMaterialSetting([](MaterialSetting* mat) {
		//mat->SetLightingEnable(false);
	});

	//��
	m_sky.Init(L"Resource/cubemap/cube2.dds");
}

Shibuya::~Shibuya()
{
}

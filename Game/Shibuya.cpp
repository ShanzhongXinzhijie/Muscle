#include "stdafx.h"
#include "Shibuya.h"


Shibuya::Shibuya()
{
	//ŠXƒ‚ƒfƒ‹
	m_model.Init(L"Resource/modelData/city.cmo");
	m_model.GetSkinModel().FindMaterialSetting([](MaterialSetting* mat) {
		//mat->SetLightingEnable(false);
	});

	//‹ó
	m_sky.Init(L"Resource/cubemap/cube2.dds");
}

Shibuya::~Shibuya()
{
}

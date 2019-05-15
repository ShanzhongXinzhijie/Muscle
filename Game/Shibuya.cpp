#include "stdafx.h"
#include "Shibuya.h"


Shibuya::Shibuya()
{
	//ŠXƒ‚ƒfƒ‹
	m_model.Init(L"Resource/modelData/tikei.cmo");//city
	m_model.GetSkinModel().FindMaterialSetting([](MaterialSetting* mat) {
		//mat->SetLightingEnable(false);
	});
	m_model.SetScale(CVector3::One()*5.0f);
	m_phyStaticObject.CreateMesh(m_model);

	//‹ó
	m_sky.Init(L"Resource/cubemap/cube2.dds");
}

Shibuya::~Shibuya()
{
}

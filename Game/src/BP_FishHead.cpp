#include "stdafx.h"
#include "BP_FishHead.h"

using namespace GameObj;

void BP_FishHead::InnerStart() {
	//���f��
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/fishhead.cmo");
}

void BP_FishHead::Update() {

}
#include "stdafx.h"
#include "PartsList.h"
#include "BP_KaniArm.h"

const PartType PartsList::m_s_partTypes[PartsList::PART_TYPE_MAXNUM] = {
	  {[]() { return std::make_unique<BP_KaniArm>(); }, L"KaniArm"},
};

const PartType& PartsList::GetPartType(const wchar_t* partName) {
	//�w��̖��O�̃p�[�c���擾
	for (auto& type : m_s_partTypes) {
		if (wcscmp(type.name, partName) == 0) {//���O�͈�v���邩
			return type;
		}
	}
	DW_ERRORBOX(true, "PartsList::GetPartType() ����ȃp�[�c�͂Ȃ�");

	return m_s_partTypes[0];
}

std::unique_ptr<IBodyPart> PartsList::CreateBodyPart(const wchar_t* partName) {
	DW_ERRORBOX(!partName, "PartsList::CreateBodyPart() �p�[�c�����w�肵�Ă��������B");

	//�w��̖��O�̃p�[�c�����[�h
	for (auto& type : m_s_partTypes) {
		if (wcscmp(type.name, partName) == 0) {//���O�͈�v���邩
			//�쐬
			return type.createPartFunc();
		}
	}
	DW_ERRORBOX(true, "PartsList::CreateBodyPart() ����ȃp�[�c�͂Ȃ�");

	return nullptr;
}
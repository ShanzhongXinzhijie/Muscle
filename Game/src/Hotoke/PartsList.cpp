#include "stdafx.h"
#include "PartsList.h"
#include "BP_KaniArm.h"

const PartType PartsList::m_s_partTypes[PartsList::PART_TYPE_MAXNUM] = {
	  {[]() { return std::make_unique<BP_KaniArm>(); }, L"KaniArm"},
};

const PartType& PartsList::GetPartType(const wchar_t* partName) {
	//指定の名前のパーツを取得
	for (auto& type : m_s_partTypes) {
		if (wcscmp(type.name, partName) == 0) {//名前は一致するか
			return type;
		}
	}
	DW_ERRORBOX(true, "PartsList::GetPartType() こんなパーツはない");

	return m_s_partTypes[0];
}

std::unique_ptr<IBodyPart> PartsList::CreateBodyPart(const wchar_t* partName) {
	DW_ERRORBOX(!partName, "PartsList::CreateBodyPart() パーツ名を指定してください。");

	//指定の名前のパーツをロード
	for (auto& type : m_s_partTypes) {
		if (wcscmp(type.name, partName) == 0) {//名前は一致するか
			//作成
			return type.createPartFunc();
		}
	}
	DW_ERRORBOX(true, "PartsList::CreateBodyPart() こんなパーツはない");

	return nullptr;
}
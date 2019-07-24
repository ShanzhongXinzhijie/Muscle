#include "stdafx.h"
#include "StageObjectGenerator.h"

int IStageObject::m_s_nextID = 0;

//–Å‚Ú‚·
void StageObjectGenerator::Clear() {
	m_objects.clear();
}

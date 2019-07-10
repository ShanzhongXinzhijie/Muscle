#include "stdafx.h"
#include "BeamModel.h"

 const BeamModel::BeamType BeamModel::m_s_beamTypes[BEAM_TYPE_MAXNUM] = {
	   {L"BLUE",{0.0f,0.0f,1.0f,1.0f}},
	   {L"Red",{1.0f,0.0f,0.0f,1.0f}},
	   {L"Yellow",{1.0f,1.0f,0.0f,1.0f}},
};
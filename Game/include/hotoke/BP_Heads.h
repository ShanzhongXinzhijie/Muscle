#pragma once

#include "BodyPartsHeader.h"

class BP_HumanHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_HumanHead;
	}

	void InnerStart()override;
	void Update()override;
};


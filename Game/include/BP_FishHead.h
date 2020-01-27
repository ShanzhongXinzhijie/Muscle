#pragma once
#include "BodyPartsHeader.h"
class BP_FishHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_FishHead;
	}

	void InnerStart()override;
	void Update()override;
};


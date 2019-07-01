#pragma once
#include "IBodyPart.h"
class BP_FishHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;

	void InnerStart()override;
	void Update()override;
};


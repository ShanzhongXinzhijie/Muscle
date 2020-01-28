#pragma once

#include "BodyPartsHeader.h"

/// <summary>
/// ヒト頭
/// </summary>
class BP_HumanHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_HumanHead;
	}

	void InnerStart()override;
};

/// <summary>
/// ナメクジ頭
/// </summary>
class BP_SnailHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_SnailHead;
	}

	void InnerStart()override;
};

/// <summary>
/// 岩頭1
/// </summary>
class BP_RockHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_RockHead;
	}

	void InnerStart()override;
};

/// <summary>
/// 岩頭2
/// </summary>
class BP_RockIIHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_RockIIHead;
	}

	void InnerStart()override;
};

/// <summary>
/// 卵頭
/// </summary>
class BP_EggHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_EggHead;
	}

	void InnerStart()override;
};

/// <summary>
/// エレファント頭
/// </summary>
class BP_ElephantHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_ElephantHead;
	}

	void InnerStart()override;
};

/// <summary>
/// ヤシの木頭
/// </summary>
class BP_PalmHead :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;
	IBodyPart* Create()override {
		return new BP_PalmHead;
	}

	void InnerStart()override;
};
#pragma once

#include "BodyPartsHeader.h"

/// <summary>
/// �q�g��
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
/// �i���N�W��
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
/// �⓪1
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
/// �⓪2
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
/// ����
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
/// �G���t�@���g��
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
/// ���V�̖ؓ�
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
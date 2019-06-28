#pragma once
#include"CDeathHotoke.h"

class IBodyPart
{
public:
	IBodyPart(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore) {}
	virtual ~IBodyPart() {}

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Draw2D() { };

protected:
	CDeathHotoke* m_ptrCore = nullptr;
};

//見た目
//毎フレームの挙動
	//アクション
	//移動
//ダメージ

//翼
	//インタラクト判定
	//移動(入力で)
	//エフェクト
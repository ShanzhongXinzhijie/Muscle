#pragma once
#include"CDeathHotoke.h"

/// <summary>
/// ボディパーツコントローラーの親クラス
/// </summary>
template <class T> 
class IBodyController {
public:
	//コンストラクタ
	IBodyController(T* ptrbody, CDeathHotoke* ptrCore) : m_ptrBody(ptrbody), m_ptrCore(ptrCore) {}
	//毎フレーム実行する処理
	virtual void Update() = 0;

protected:
	//操作するボディパーツのポインタ
	T* m_ptrBody = nullptr;
	//本体のポインタ
	CDeathHotoke* m_ptrCore = nullptr;
};

/// <summary>
/// ボディパーツの親クラス
/// </summary>
class IBodyPart
{
public:
	IBodyPart(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore) {}
	virtual ~IBodyPart() {}

	void Start() { InnerStart(); UpdateTRS(); }
	virtual void Update() {};
	virtual void UpdateTRS() {
		if (m_model) {
			m_model->SetPos(m_ptrCore->GetPos() + m_localPos);
			m_model->SetRot(m_localRot*m_ptrCore->GetRot());
			m_model->SetScale(m_ptrCore->GetScale()*m_localScale);
		}
	}
	virtual void PostUTRSUpdate() {};
	virtual void PostLoopUpdate() {};
	virtual void Draw2D() { };

private:
	virtual void InnerStart() = 0;
	
protected:
	//本体のポインタ
	CDeathHotoke* m_ptrCore = nullptr;

	//モデル
	std::unique_ptr<GameObj::CSkinModelRender> m_model;

	//座標等
	CVector3 m_localPos, m_localScale = 1.0f;
	CQuaternion m_localRot;
};
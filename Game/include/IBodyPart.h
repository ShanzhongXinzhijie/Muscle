#pragma once

class CDeathHotoke;

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
	IBodyPart() = default;
	IBodyPart(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore) {};
	virtual ~IBodyPart() {};

	//初期化
	void Init(CDeathHotoke* ptrCore) {
		m_ptrCore = ptrCore;
	}
	   
	//ゲームオブジェクト関数
	void Start();
	virtual void Update() {}
	virtual void UpdateTRS();
	virtual void PostUTRSUpdate() {}
	virtual void PostLoopUpdate() {}
	virtual void Draw2D() {}

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
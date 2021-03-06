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
	void Update() {
		if (!m_ptrCore->GetIsControl()) {
			return;
		}
		InnerUpdate();
	}
	virtual void InnerUpdate() = 0;

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

	//自分自身を作成する仮想関数
	virtual IBodyPart* Create()const = 0;

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

	//最終的な回転クォータニオンを取得
	CQuaternion GetFinalRot()const;

	//パーツ名を取得
	const wchar_t* GetName()const {
		return m_name.c_str();
	}

private:
	//Start関数内で実行される仮想関数
	virtual void InnerStart() = 0;

protected:
	//腕を指定の座標へ向ける処理
	CVector3 TurnArm(const CVector3& armTargetPos, const CVector3& armTipPos, const CVector3& armAxis, float angleLimitFront, float angleLimitBack, float armSpeed, CVector3& localTargetPos);
	
protected:
	//本体のポインタ
	CDeathHotoke* m_ptrCore = nullptr;

	//モデル
	std::unique_ptr<GameObj::CSkinModelRender> m_model;

	//座標等
	CVector3 m_localPos, m_localScale = 1.0f;
	CQuaternion m_localRot;

	//名前
	std::wstring m_name = L"UNKNOWN";
};
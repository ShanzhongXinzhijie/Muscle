#pragma once

class CDeathHotoke;

/// <summary>
/// AIの生み出すステータス
/// </summary>
struct AIStatus {
	bool isMovingToTarget = false;//目標に移動するか?
	CVector3 moveTargetPosition;//移動したい位置
	bool isAttackingTarget = false;//目標を攻撃するか?
};

/// <summary>
/// AIのインターフェース
/// </summary>
class IAI
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="ptrCore">本体のポインタ</param>
	IAI(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore){};
	IAI() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IAI() {};

	/// <summary>
	/// 自分を作成
	/// </summary>
	virtual IAI* Create(CDeathHotoke* ptrCore = nullptr)const = 0;

	/// <summary>
	/// 毎フレームよぶやつ
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// ステータス出力
	/// </summary>
	const AIStatus& GetOutputStatus()const { return m_outputStatus; }

protected:
	CDeathHotoke* m_ptrCore = nullptr;
	AIStatus m_outputStatus;
};

/// <summary>
/// テストAI
/// </summary>
class TestAI : public IAI {
public:
	using IAI::IAI;

	IAI* Create(CDeathHotoke* ptrCore = nullptr)const override {
		return new TestAI(ptrCore ? ptrCore : m_ptrCore);
	}

	void Update()override;
};
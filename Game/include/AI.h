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
	IAI(CDeathHotoke* ptrCore) : m_ptrCore(ptrCore){};
	virtual ~IAI() {};

	virtual void Update() = 0;

	/// <summary>
	/// ステータス出力
	/// </summary>
	const AIStatus& GetOutputStatus()const { return m_outputStatus; }

protected:
	CDeathHotoke* m_ptrCore = nullptr;
	AIStatus m_outputStatus;
};

class TestAI : public IAI {
public:
	using IAI::IAI;
	void Update()override;
};
#pragma once
#include"IGamePad.h"

class IBodyPart;

class CDeathHotoke :
	public IGameObject
{
public:
	CDeathHotoke(IGamePad* ptrPad):m_ptrPad(ptrPad){}

	bool Start()override;
	void Update()override;
	void PostRender()override;

	//移動量を加える
	void AddMove(const CVector3& vec) { m_move += vec; }
	void AddRot(const CQuaternion& rot) { m_rotMove = rot * m_rotMove; }

	//セッター
	void SetPos(const CVector3& pos) { m_pos = pos; }
	void SetRot(const CQuaternion& rot) { m_rot = rot; }

	void SetTargetPos(const CVector3& pos) { m_targetPos = pos; }
	
	//ゲッター
	const CVector3& GetPos()const { return m_pos; }	
	const CQuaternion& GetRot()const { return m_rot; }
	const CVector3& GetScale()const { return m_scale; }

	const CVector3& GetTargetPos() { return m_targetPos; }

	//パッドの取得
	IGamePad* GetPad() { return m_ptrPad; }

private:
	//コアのモデル
	GameObj::CSkinModelRender m_coreModel;

	//TRS
	CVector3 m_pos, m_scale;
	CQuaternion m_rot;

	//移動量
	CVector3 m_move;
	CQuaternion m_rotMove;

	//パーツ
	IBodyPart* m_parts[4] = {};

	//ゲームパッド
	IGamePad* m_ptrPad = nullptr;

	//ターゲット位置
	CVector3 m_targetPos;

public:
	//ボディパーツの種類
	enum enBodyParts {
		enWing, enLeg, enArm, enHead, enPartsNum, 
	};
};


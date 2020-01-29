#pragma once

#include"CDeathHotoke.h"

/// <summary>
/// デスホトケのアセンブル情報を管理
/// </summary>
class HotokeAssembleManager : public IGameObject {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	bool Start()override {
		SetName(L"HotokeAssembleManager");
		return true;
	}

	/// <summary>
	/// デスホトケのアセンブル情報
	/// </summary>
	struct HotokeAssemble {
		IBodyPart* parts[CDeathHotoke::enPartsNum] = {};
		IAI* ai = nullptr;
	};

	/// <summary>
	/// アセンブル情報を取得
	/// </summary>
	/// <param name="playerNum">プレイヤー番号</param>
	HotokeAssemble& GetHotokeAssemble(int playerNum) {
		return m_assemble[playerNum];
	}

private:
	HotokeAssemble m_assemble[PLAYER_NUM];//アセンブル設定
};

/// <summary>
/// アセンブル画面
/// </summary>
class AssembleScene :
	public IGameObject
{
public:
	AssembleScene(int playerNum):m_playerNum(playerNum){}

	bool Start()override;

	void Update()override;
	void PostUpdate()override;

	//void PostLoopUpdate()override;
	void HUDRender(int)override;
	//void PostRender()override;

private:
	int m_playerNum = PLAYER_NUM;

	bool m_isReady[PLAYER_NUM] = {};
	int m_nowEditBody[PLAYER_NUM] = {};
	int m_nowBodyType[PLAYER_NUM][CDeathHotoke::enPartsNum] = {};

	CFont m_font, m_fontBold;
	std::vector< std::unique_ptr<CDeathHotoke>> m_hotokes;
	std::vector<std::unique_ptr<GameObj::PerspectiveCamera>> m_cameras;
	CVector3 m_toCameraPos[PLAYER_NUM];
	CVector2 m_cameraRots[PLAYER_NUM];
	GameObj::CDirectionLight m_light;

	std::vector<std::unique_ptr<IBodyPart>> m_parts[CDeathHotoke::enPartsNum];
};


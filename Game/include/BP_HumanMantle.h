#pragma once
#include "BodyPartsHeader.h"

/// <summary>
/// マント
/// </summary>
class BP_HumanMantle :
	public IBodyPart
{
public:
	using IBodyPart::IBodyPart;

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void PostLoopUpdate()override;

	//アクション
	void Move(const CVector2& dir);//移動
	void Step(const CVector2& dir);//ステップ移動
	void Jump();//ジャンプ
	void Parachute();//パラシュート(落下速度軽減)

private:
	//コントローラー
	IBodyController<BP_HumanMantle>* m_controller = nullptr;

	btSoftBody* m_cloth = nullptr;
	
	typedef std::vector<DirectX::VertexPositionNormalTangentColorTexture> VertexPosArray;
	std::vector<std::unique_ptr<VertexPosArray>> m_vertexPosArrays;
	struct NodeInfo
	{
		btSoftBody::Node* node;
		CVector3 offset;
	};
	typedef std::vector<NodeInfo> NodeArray;
	std::vector<std::unique_ptr<NodeArray>> m_nodeArrays;
};

/// <summary>
/// マント #人コントローラー
/// </summary>
class HCon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void Update()override;
};
/// <summary>
/// マント #AIコントローラー
/// </summary>
class AICon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void Update()override;
};
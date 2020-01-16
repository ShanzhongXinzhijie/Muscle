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

	//ソフトボディ
	btSoftBody* m_cloth = nullptr;
	
	//頂点更新系
	typedef std::vector<DirectX::VertexPositionNormalTangentColorTexture> VertexPosArray;	//頂点情報の配列
	std::vector<std::unique_ptr<VertexPosArray>> m_vertexPosArrays;							//頂点情報の配列の配列

	typedef std::vector<unsigned int> IndexBuffer;					//インデックスバッファ
	std::vector<std::unique_ptr<IndexBuffer>> m_indexBufferArray;	//インデックスバッファの配列

	struct NodeInfo	//ノード情報
	{
		btSoftBody::Node* node;
		CVector3 offset;
	};
	typedef std::vector<NodeInfo> NodeArray;				//ノード情報の配列
	std::vector<std::unique_ptr<NodeArray>> m_nodeArrays;	//ノード情報の配列の配列
};

/// <summary>
/// マント #人コントローラー
/// </summary>
class HCon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};
/// <summary>
/// マント #AIコントローラー
/// </summary>
class AICon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};
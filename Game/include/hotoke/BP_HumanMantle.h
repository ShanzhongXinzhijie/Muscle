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
	IBodyPart* Create()const override {
		return new BP_HumanMantle;
	}

	~BP_HumanMantle() {
		if (m_cloth) {
			btCollisionObject* obj = m_cloth;

			// オブジェクトがRigid Bodyの場合の破棄
			btRigidBody* body = btRigidBody::upcast(obj);
			if (body && body->getMotionState()) {
				delete body->getMotionState();
			}

			// オブジェクトがSoft Bodyの場合の破棄
			GetPhysicsWorld().GetDynamicWorld()->removeSoftBody(m_cloth);

			GetPhysicsWorld().GetDynamicWorld()->removeCollisionObject(m_cloth);

			delete m_cloth;
		}
	}

	void InnerStart()override;
	void Update()override;
	void PostUTRSUpdate()override;
	void PostLoopUpdate()override;
	void Draw2D()override;

	//アクション
	void Move(const CVector2& dir);//移動
	void Stop();//停止
	void Step(const CVector2& dir);//ステップ移動
	void Yaw(float lerp);//旋回

private:
	//コントローラー
	IBodyController<BP_HumanMantle>* m_controller = nullptr;

	CVector3 m_moveDir;//移動方向
	float m_yawInertia = 0.f;//回転慣性
	int m_invincibleFrame = 0;//無敵時間
	int m_cooldownFrame = 0;//クールダウン

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
private:
	bool m_isStickNeutral = true;
};
/// <summary>
/// マント #AIコントローラー
/// </summary>
class AICon_HumanMantle : public IBodyController<BP_HumanMantle> {
public:
	using IBodyController::IBodyController;
	void InnerUpdate()override;
};
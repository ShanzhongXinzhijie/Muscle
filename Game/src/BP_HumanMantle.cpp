#include "stdafx.h"
#include "BP_HumanMantle.h"

using namespace GameObj;

void BP_HumanMantle::InnerStart() {
	//物理ワールド設定
	//TODO　別の所テ゛
	GetPhysicsWorld().SetGravity({0.0f,-10.0f,0.0f});	
	//GetPhysicsWorld().GetSoftBodyWorldInfo()->air_density = 1.2f*10.0f;

	//布作る
	btScalar sl = 500;// 8710.938;// 500.0;//8710.938 ~ 169.777
	btScalar y = 0.0f;// 1200.0;
	int res = 9;
	btSoftBody* cloth = btSoftBodyHelpers::CreatePatch(*GetPhysicsWorld().GetSoftBodyWorldInfo(),
		btVector3(-sl, -sl, y),    // 四隅の座標 00
		btVector3(-sl, sl, y),    // 四隅の座標 10
		btVector3(sl, -sl, y),    // 四隅の座標 01
		btVector3(sl, sl, y),    // 四隅の座標 11
		res, res, // 分割数(2方向)
		2 + 8, // 四隅の固定フラグ(1,2,4,8)
		true); // 斜め方向のばねのON/OFF

	cloth->getCollisionShape()->setMargin(0.5);
	cloth->setTotalMass(150); // 全体の質量

	cloth->m_cfg.kMT = 0.8f;//元の形状を保とうとする力を働かせる
	//cloth->m_cfg.kDP = 0.2f;//空気抵抗
	
	btSoftBody::Material* pm = cloth->appendMaterial();//m_materials[0]
	pm->m_kLST = 0.4f;//粘性(0~1
	cloth->generateBendingConstraints(2, pm);

	int nodenum = cloth->m_nodes.size();
	for (int i = 0; i < nodenum; ++i) {
		//X
		float distance = abs((-sl) - cloth->m_nodes.at(i).m_x.getY()) / (sl*2.0f);
		distance = 1.0f - distance;
		constexpr float const_scale = 0.01948929036115284025669795835994f * 10.0f;
		cloth->m_nodes.at(i).m_x.setX(cloth->m_nodes.at(i).m_x.getX()*(const_scale + (1.0f- const_scale)*distance));
		if (distance < FLT_EPSILON) { cloth->setMass(i, 0.f); }
		//Z
	}

	//ソフトボディを登録
	GetPhysicsWorld().GetDynamicWorld()->addSoftBody(cloth);

	btTransform trans;
	trans.setIdentity();		// 位置姿勢行列の初期化
	trans.setOrigin(btVector3(0, 2000, 0));		// 初期位
	cloth->transform(trans);

	m_cloth = cloth;
	
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/manto.cmo");
	m_model->GetSkinModel().SetCullMode(D3D11_CULL_NONE);//バックカリングしない
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetIsUseTexZShader(true);//シャドウマップ描画にテクスチャ使う
		}
	);

	//頂点操作用情報を作成
	ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();
	m_model->GetSkinModel().FindMesh([&](const auto& mesh) {
			D3D11_MAPPED_SUBRESOURCE subresource;

			//頂点のロード
			HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
			if (FAILED(hr)) { return; }

			D3D11_BUFFER_DESC bufferDesc;
			mesh->vertexBuffer->GetDesc(&bufferDesc);
			int vertexCount = bufferDesc.ByteWidth / mesh->vertexStride;//頂点数
			char* pData = reinterpret_cast<char*>(subresource.pData);
			std::unique_ptr<VertexPosArray> vertexBuffer = std::make_unique<VertexPosArray>();
			std::unique_ptr<NodeArray> nodes = std::make_unique<NodeArray>();
			for (int i = 0; i < vertexCount; i++) {
				//頂点情報の保存
				vertexBuffer->emplace_back(*reinterpret_cast<DirectX::VertexPositionNormalTangentColorTexture*>(pData));
				nodes->emplace_back();
				float closestDistanse = -1.0f;
				for (int i = 0; i < nodenum; ++i) {
					CVector3 disvec = CVector3(vertexBuffer->back().position) - (CVector3)cloth->m_nodes.at(i).m_x;
					float distance = disvec.LengthSq();
					if (closestDistanse < 0.0f || closestDistanse > distance) {
						nodes->back().node = &cloth->m_nodes.at(i);
						nodes->back().offset = disvec;
					}
				}				
				pData += mesh->vertexStride;//次の頂点へ
			}
			//保存
			m_vertexPosArrays.push_back(std::move(vertexBuffer));
			m_nodeArrays.push_back(std::move(nodes));

			//頂点バッファをアンロック
			deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);
		}
	);
	
}

void BP_HumanMantle::Update() {
	if (GetKeyInput(VK_UP)) {
		m_cloth->setVelocity(btVector3(1000, 1, 0));
	}
	if (GetKeyInput(VK_DOWN)) {
		m_cloth->addForce(btVector3(-1000, 1, 0));
	}
}

void BP_HumanMantle::PostLoopUpdate() {
	//頂点バッファ更新
	{
		int i = 0;
		for (auto& Varray : m_vertexPosArrays) {
			int i2 = 0;
			for (auto& vertex : *Varray) {
				auto& nodearray = *m_nodeArrays[i];
				vertex.position.x = nodearray[i2].node->m_x.x() + nodearray[i2].offset.x;
				vertex.position.y = nodearray[i2].node->m_x.y() + nodearray[i2].offset.y;
				vertex.position.z = nodearray[i2].node->m_x.z() + nodearray[i2].offset.z;
				i2++;
			}
			i++;
		}
	}

	//頂点バッファ適用
	int i = 0;
	ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();
	m_model->GetSkinModel().FindMesh([&](const auto& mesh) {
		//頂点バッファを作成。
		D3D11_MAPPED_SUBRESOURCE subresource;
		//頂点の書き込み
		HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
		if (FAILED(hr)) { return; }
		memcpy(subresource.pData, &m_vertexPosArrays[i].get()->front(), sizeof(DirectX::VertexPositionNormalTangentColorTexture) * m_vertexPosArrays[i].get()->size()); //コピー				
		deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);
		i++;
		}
	);
}
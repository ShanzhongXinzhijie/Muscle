#include "stdafx.h"
#include "BP_HumanMantle.h"

using namespace GameObj;

namespace {
	constexpr int INVINCIBLE_FRAME = 15;//ステップ無敵時間
	constexpr int COOLDOWN_FRAME = 30;//ステップクールダウン
	constexpr int DONTMOVE_FRAME = 10;//移動不可時間
}

void BP_HumanMantle::InnerStart() {
	m_name = L"マント";

	//落下速度変更
	m_ptrCore->MulGravity(0.4f);

	//布作る
	btScalar sl = 500.0f;//8710.938 ~ 169.777
	btScalar y = 0.0f;// 1200.0;
	constexpr int res = 9;//分割数
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

	//頂点設定
	int nodenum = cloth->m_nodes.size();
	for (int i = 0; i < nodenum; ++i) {
		float distance = abs((-sl) - cloth->m_nodes.at(i).m_x.getY()) / (sl*2.0f);
		distance = 1.0f - distance;

		//形状
		cloth->m_nodes.at(i).m_x.setX(cloth->m_nodes.at(i).m_x.getX() * max(0.1f,distance));
		cloth->m_nodes.at(i).m_x.setY(cloth->m_nodes.at(i).m_x.getY() * 0.5f);

		//固定
		if (distance < FLT_EPSILON) { cloth->setMass(i, 0.f); }
	}

	//ソフトボディを登録
	GetPhysicsWorld().GetDynamicWorld()->addSoftBody(cloth, btBroadphaseProxy::DefaultFilter, 0);

	//ポインタをメンバにコピー
	m_cloth = cloth;
	
	//表示モデル位置調整
	m_localPos.y = -150.0f*0.25f;
	m_localPos.z = -65.0f;
	m_localScale *= 15.0f;

	//モデルロード
	m_model = std::make_unique<CSkinModelRender>();
	m_model->Init(L"Resource/modelData/manto_test.cmo", enFbxUpAxisY, enFbxRightHanded, false);
	//テクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;
	TextureFactory::GetInstance().Load(L"Resource/texture/mant.png", nullptr, &textureView, nullptr, true);
	//マテリアル設定
	m_model->GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetAlbedoTexture(textureView.Get());
			mat->SetAlbedoScale({1.0f,0.01f,0.08f,0.9f});//半透明
			mat->SetIsUseTexZShader(true);//シャドウマップ描画にテクスチャ使う
			mat->SetShininess(0.08f);
		}
	);
	//m_model->SetIsShadowCaster(false);//TODO オフセットにする(model.fxで
	m_model->GetSkinModel().SetIsFrustumCulling(false);//視錐台カリングをオフ
	//ポストドロー設定
	//m_model->InitPostDraw(PostDrawModelRender::enAlpha);

	//頂点操作用情報を作成
	ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();
	m_model->GetSkinModel().FindMesh([&](const auto& mesh) {
			//頂点情報の作成
			{
				D3D11_MAPPED_SUBRESOURCE subresource;

				//頂点のロード
				HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
				if (FAILED(hr)) { 
					return;
				}

				D3D11_BUFFER_DESC bufferDesc;
				mesh->vertexBuffer->GetDesc(&bufferDesc);
				int vertexCount = bufferDesc.ByteWidth / mesh->vertexStride;//頂点数
				char* pData = reinterpret_cast<char*>(subresource.pData);
				std::unique_ptr<VertexPosArray> vertexBuffer = std::make_unique<VertexPosArray>();
				std::unique_ptr<NodeArray> nodes = std::make_unique<NodeArray>();
				for (int i = 0; i < vertexCount; i++) {
					//頂点情報の保存
					vertexBuffer->emplace_back(*reinterpret_cast<DirectX::VertexPositionNormalTangentColorTexture*>(pData));

					float distance = abs((-sl) - vertexBuffer->back().position.y) / (sl*2.0f);
					distance = 1.0f - distance;
					vertexBuffer->back().position.x *= max(0.1f, distance);
					vertexBuffer->back().position.y *= 0.5f;

					nodes->emplace_back();
					float closestDistanse = -1.0f;
					for (int i = 0; i < nodenum; ++i) {
						CVector3 disvec = CVector3(vertexBuffer->back().position) - (CVector3)cloth->m_nodes.at(i).m_x;
						float distance = disvec.LengthSq();
						if (closestDistanse < 0.0f || closestDistanse > distance) {
							nodes->back().node = &cloth->m_nodes.at(i);
							nodes->back().offset = disvec;
							closestDistanse = distance;
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
			//インデックスバッファを作成			
			{
				D3D11_MAPPED_SUBRESOURCE subresource;

				//インデックスバッファをロック
				HRESULT hr = deviceContext->Map(mesh->indexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
				if (FAILED(hr)) { 
					return;
				}

				D3D11_BUFFER_DESC bufferDesc;
				mesh->indexBuffer->GetDesc(&bufferDesc);
				std::unique_ptr<IndexBuffer> indexBuffer = std::make_unique<IndexBuffer>();
				int stride = 2;
				int indexCount = bufferDesc.ByteWidth / stride;
				unsigned short* pIndex = reinterpret_cast<unsigned short*>(subresource.pData);
				for (int i = 0; i < indexCount; i++) {
					indexBuffer->push_back(pIndex[i]);
				}
				
				//インデックスバッファをアンロック
				deviceContext->Unmap(mesh->indexBuffer.Get(), 0);
				
				//保存
				m_indexBufferArray.push_back(std::move(indexBuffer));
			}
		}
	);
	
	//コントローラー
	if (m_ptrCore->GetPad()) {
		m_controller = new HCon_HumanMantle(this, m_ptrCore);
	}
	else {
		m_controller = new AICon_HumanMantle(this, m_ptrCore);
	}
}

void BP_HumanMantle::Update() {
	//コントローラーに操作させる
	m_controller->Update();

	//移動
	CVector3 move = m_moveDir*7.0f;
	if (m_invincibleFrame > 0) {//ステップ
		move *= 6.0f;
		m_yawInertia = 0.f;//旋回無効
	}
	if (m_cooldownFrame > COOLDOWN_FRAME - DONTMOVE_FRAME) {
		move *= 0.0f;
	}
	//move.y = move.Length()*1.125f*CalcAirScale(m_ptrCore->GetHeightMeter());
	//m_ptrCore->GetRot().Multiply(move);
	m_ptrCore->AddVelocity(move);

	//旋回
	if (abs(m_yawInertia) > FLT_EPSILON) {
		m_ptrCore->AddAngularVelocity(CVector3::AxisY(), m_yawInertia);//旋回
		m_ptrCore->AddVelocity(CVector3::AxisY()*abs(m_yawInertia)*500.0f*CalcAirScale(m_ptrCore->GetHeightMeter()));//上昇
		m_yawInertia *= 0.57f;//慣性// (abs(m_yawInertia) - CMath::DegToRad(0.1f)) / abs(m_yawInertia);
	}

	//クールダウン
	if (m_cooldownFrame > 0) {
		m_cooldownFrame--;
	}
	//無敵時間
	if (m_invincibleFrame > 0) {
		m_ptrCore->Invincible();//無敵化
		m_invincibleFrame--;
		if (m_invincibleFrame <= 0) {
			//クールダウン
			m_cooldownFrame = COOLDOWN_FRAME;
		}
	}
}

void BP_HumanMantle::PostUTRSUpdate() {
	//マントを動かす
	//移動量
	CVector3 force = m_ptrCore->GetMove()*-1000.0f;
	//方向変換(マントは向き変わらないため)
	GetFinalRot().InverseMultiply(force);
	//移動量をソフトボディに適用
	m_cloth->addForce(force);

	//ソフトボディ移動制限
	int nodenum = m_cloth->m_nodes.size();
	constexpr int res = 9;//分割数
	for (int i = 0; i < nodenum; ++i) {
		if (m_cloth->m_nodes.at(i).m_x.z() > -50.0f*(res - (i % res) - 1)) {
			m_cloth->m_nodes.at(i).m_x.setZ(-50.0f*(res - (i % res)) - 1);
			m_cloth->m_nodes.at(i).m_v = btVector3(0, 0, 0);// setVelocity(btVector3(0, 0, 0));
		}
	}
}

void BP_HumanMantle::PostLoopUpdate() {
	//頂点バッファ更新
	{
		//頂点位置更新
		int i = 0;
		for (auto& Varray : m_vertexPosArrays) {
			int i2 = 0;
			for (auto& vertex : *Varray) {
				auto& nodearray = *m_nodeArrays[i];
				vertex.position.x = (nodearray[i2].node->m_x.x() + nodearray[i2].offset.x)*-1.0f;
				vertex.position.y = nodearray[i2].node->m_x.y() + nodearray[i2].offset.y;
				vertex.position.z = nodearray[i2].node->m_x.z() + nodearray[i2].offset.z;
				i2++;
			}
			i++;
		}

		//法線更新
		i = 0;
		for (auto& Iarray : m_indexBufferArray) {
			int i2 = 0;
			int indexs[3];
			for (auto& index : *Iarray) {
				indexs[i2] = index;
				i2++;
				if (i2 == 3) {
					//法線計算
					CVector3 A = (*m_vertexPosArrays[i])[indexs[0]].position;
					CVector3 B = (*m_vertexPosArrays[i])[indexs[1]].position;
					CVector3 C = (*m_vertexPosArrays[i])[indexs[2]].position;
					CVector3 AB(B - A);
					CVector3 BC(C - B);
					AB.Cross(BC);
					AB.Normalize();
					AB *= -1.0f;//反転
					(*m_vertexPosArrays[i])[indexs[0]].normal = DirectX::XMFLOAT3(AB.x, AB.y, AB.z);
					(*m_vertexPosArrays[i])[indexs[1]].normal = DirectX::XMFLOAT3(AB.x, AB.y, AB.z);
					(*m_vertexPosArrays[i])[indexs[2]].normal = DirectX::XMFLOAT3(AB.x, AB.y, AB.z);

					//tangent計算
					if (abs(AB.y) > 0.5f) {
						AB.Cross(CVector3::AxisZ());
					}
					else {
						AB.Cross(CVector3::AxisY());
					}
					(*m_vertexPosArrays[i])[indexs[0]].tangent = DirectX::XMFLOAT4(AB.x, AB.y, AB.z, 1.0f);
					(*m_vertexPosArrays[i])[indexs[1]].tangent = DirectX::XMFLOAT4(AB.x, AB.y, AB.z, 1.0f);
					(*m_vertexPosArrays[i])[indexs[2]].tangent = DirectX::XMFLOAT4(AB.x, AB.y, AB.z, 1.0f);

					i2 = 0;
				}
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

void BP_HumanMantle::Draw2D() {
	if (!m_ptrCore->GetIsDrawHUD()) { return; }

	//m_ptrCore->GetFont()->DrawFormat(L"Deg: %.1f", { 0.0f,0.85f },0.0f,CMath::RadToDeg(m_ptrCore->GetPad()->GetStickRollAngle(L)));

	//ステップ表示
	if (m_invincibleFrame > 0) {
		m_ptrCore->GetFont()->Draw(L"[INVINCIBLE]", 0.5f, 0.5f);// { 0.3f, 0.90f }, { 0.0f,0.0f });
	}
	if (m_cooldownFrame > 0) {
		m_ptrCore->GetWarningFont()->Draw(L"[COOLDOWN]", 0.5f, 0.5f);//, { 0.3f,0.90f }, { 0.0f,0.0f });
	}

	//限界高度警告
	if (m_ptrCore->GetHeightMeter() > LIMIT_HEIGHT_METER) {
		m_ptrCore->GetWarningFont()->Draw(L"[WARNING-HEIGHT]", { 0.7f,0.90f }, { 1.0f,0.0f });
	}
}

void BP_HumanMantle::Move(const CVector2& dir) {
	//if (CMath::Square(m_moveDir.Length()-0.1f) < dir.LengthSq()) {
		m_moveDir = CVector3(dir.x, 0.0f, dir.y); 
	//}
}
void BP_HumanMantle::Step(const CVector3& dir) {
	if (m_invincibleFrame > 0 || m_cooldownFrame > 0) {
		return;
	}

	//ステップ移動
	m_moveDir = dir;

	//無敵時間
	m_invincibleFrame = INVINCIBLE_FRAME;
}
void BP_HumanMantle::Stop() {
	m_moveDir = CVector3::Zero();
}
void BP_HumanMantle::Yaw(float angle) {
	if (abs(m_yawInertia) < abs(angle) || m_yawInertia > 0.0f && angle < 0.0f || m_yawInertia < 0.0f && angle > 0.0f) {
		m_yawInertia = angle;
	}
}

//コントローラー
void HCon_HumanMantle::InnerUpdate() {
	if (m_ptrCore->GetPad()->GetStick(L).LengthSq() < CMath::Square(0.24f)) {
		m_isStickNeutral = true;
	}

	if (m_ptrCore->GetPad()->IsSmashInput(L)) {
		//ステップ
		CVector3 viewDir = m_ptrCore->GetVanisingPoint() - m_ptrCore->GetPos(); viewDir.y = 0.0f;
		if (viewDir.LengthSq() > FLT_EPSILON) {
			viewDir.Normalize();
			CVector3 moveDir;
			moveDir += viewDir * m_ptrCore->GetPad()->GetStick(L).y;
			moveDir += viewDir.GetCross(CVector3::AxisY())*-m_ptrCore->GetPad()->GetStick(L).x;
			moveDir.Normalize();
			m_ptrBody->Step(moveDir);
		}
	}
	else if(m_isStickNeutral && m_ptrCore->GetPad()->GetStick(L).LengthSq() > CMath::Square(0.24f)){
		//移動
		//m_ptrBody->Move(m_ptrCore->GetPad()->GetStick(L).GetNorm());
		m_isStickNeutral = false;
	}

	//旋回
	m_ptrBody->Yaw(m_ptrCore->GetPad()->GetStickRollAngle(L)*0.1f);

	//ストップ
	if (m_ptrCore->GetPad()->GetWingInput()) {
		m_ptrBody->Stop();
	}
}

void AICon_HumanMantle::InnerUpdate() {}
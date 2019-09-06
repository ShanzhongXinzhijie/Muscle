#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"

void CDeathHotoke::CalcDirection() {
	m_front = CVector3::Front(); GetRot().Multiply(m_front); m_back = m_front * -1.0f;
	m_left = CVector3::Left(); GetRot().Multiply(m_left); m_right = m_left * -1.0f;
	m_up = CVector3::Up(); GetRot().Multiply(m_up); m_down = m_up * -1.0f;
}

bool CDeathHotoke::Start() {
	//スケール
	constexpr float modelScale = 0.0188f*2.0f;
	m_scale = { modelScale };

	//コア生成
	m_coreModel.Init(L"Resource/modelData/core.cmo");	
	//ノーマルマップ適用
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;	
	TextureFactory::GetInstance().Load(L"Resource/normalMap/ngasi.png", nullptr, &textureView, nullptr, true);
	m_coreModel.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.9f);
			mat->SetTranslucent(0.2f);
		}
	);

	//当たり判定
	m_col.m_collision.CreateMesh({}, {}, m_scale, m_coreModel.GetSkinModel());//TODO 球にしようか
	m_col.m_collision.SetIsHurtCollision(true);//これは喰らい判定
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//クラス取り出す
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				//ダメージ
				Damage(*H);
			}
		}
	);
	
	//位置初期化
	m_pos = CVector3::AxisY()*1000.0f;
	m_pos.z += 200.0f;
	
	//パーツ生成
	m_parts[enWing] = new BP_BirdWing(this);
	m_parts[enLeg] = new BP_HumanLeg(this);
	m_parts[enArm] = new BP_KaniArm(this);
	m_parts[enHead] = new BP_FishHead(this);
	//パーツのStart
	for (auto& part : m_parts) {
		if (part)part->Start();
	}
	

	m_coreModel.GetSkinModel().FindMesh([&](const auto& mesh) {
		ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();
		{
			D3D11_MAPPED_SUBRESOURCE subresource;

			//頂点のロード
			HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
			if (FAILED(hr)) { return; }			

			D3D11_BUFFER_DESC bufferDesc;
			mesh->vertexBuffer->GetDesc(&bufferDesc);
			int vertexCount = bufferDesc.ByteWidth / mesh->vertexStride;//頂点数
			char* pData = reinterpret_cast<char*>(subresource.pData);
			VertexPositionPtr vertexBuffer = std::make_unique<VertexPosition>();
			for (int i = 0; i < vertexCount; i++) {
				vertexBuffer->emplace_back(*reinterpret_cast<DirectX::VertexPositionNormalTangentColorTexture*>(pData));
				vertexBuffer->back().position.x += CMath::RandomZeroToOne()*1000.0f;
				vertexBuffer->back().position.y += CMath::RandomZeroToOne()*1000.0f;
				vertexBuffer->back().position.z += CMath::RandomZeroToOne()*1000.0f;
				//次の頂点へ。
				pData += mesh->vertexStride;
			}

			//頂点バッファをアンロック
			deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);
			m_vertexBufferArray.push_back(std::move(vertexBuffer));
		}
	}
	);

	CMatrix mat,mat2;
	mat2.MakeTranslation(CVector3::Up()*1200.0f);
	mat.MakeScaling(m_scale);
	mat.Mul(mat,mat2);
	m_meshdata.CreateFromSkinModel(m_coreModel.GetSkinModel(),&mat);

	int vertex_count = (int)m_meshdata.GetVertexBuffer().size(); // 総頂点数
	int index_count = 0;
	for (int i = 0; i < m_meshdata.GetPolygonNum(); i++) {
		index_count += (int)m_meshdata.GetPolygonIndexs(i).size(); // 総ポリゴン数
	}
	index_count /= 3;
	btScalar *vertices = new btScalar[vertex_count * 3]; // 頂点座標を格納する配列
	int *indices = new int[index_count * 3]; // ポリゴンを構成する頂点番号を格納する配列

	// 頂点座標の取り出し
	for (int i = 0; i < vertex_count; ++i) {
		vertices[3 * i] = m_meshdata.GetVertexBuffer()[i].x;
		vertices[3 * i + 1] = m_meshdata.GetVertexBuffer()[i].y;
		vertices[3 * i + 2] = m_meshdata.GetVertexBuffer()[i].z;
	}
	// ポリゴンを構成する頂点番号の取り出し
	/*for (int i = 0; i < index_count; ++i) {
		indices[3 * i] = m_meshdata.GetPolygonIndexs(i)[0];
		indices[3 * i + 1] = m_meshdata.GetPolygonIndexs(i)[1];
		indices[3 * i + 2] = m_meshdata.GetPolygonIndexs(i)[2];
	}*/
	int i2 = 0;
	for (int i = 0; i < m_meshdata.GetPolygonNum(); i++) {
		for (auto& ind : m_meshdata.GetPolygonIndexs(i)) {
			indices[i2] = ind;
			i2++;
		}
	}

	//ソフトゥボデー作成
	btSoftBody* soft_body = btSoftBodyHelpers::CreateFromTriMesh(*GetPhysicsWorld().GetSoftBodyWorldInfo(), vertices, indices, index_count);
	soft_body->getCollisionShape()->setMargin(0.01);
	soft_body->setTotalMass(10.0); // 全体の質量
	soft_body->m_materials[0]->m_kLST = 0.5;
	int nodenum = soft_body->m_nodes.size();
	//soft_body->m_nodes.at(0).m_x;//座標
	for (int i = 0; i < nodenum /2; ++i) {
		soft_body->setMass(i, 0.f);
	}
	soft_body->randomizeConstraints();
	soft_body->setPose(true, true);

	soft_body->m_cfg.kLF = 0.05;
	soft_body->m_cfg.kDG = 0.01;

	GetPhysicsWorld().GetDynamicWorld()->addSoftBody(soft_body);
	soft_body->addForce(btVector3(1, 0, 0));
	
	m_soft_body = soft_body;

	return true;
}

void CDeathHotoke::Update() {

	if (GetKeyInput(VK_UP)) {
		m_soft_body->addForce(btVector3(1, 1, 0));
	}
	if (GetKeyInput(VK_DOWN)) {
		m_soft_body->addForce(btVector3(-1, 1, 0));
	}

	//AI実行
	if (m_ai) { m_ai->Update(); }

	//移動適応
	m_pos += m_move;
	m_rot = m_rotMove * m_rot;
	//減速
	m_move *= 0.5f;
	m_rotMove.Slerp(0.5f, m_rotMove, CQuaternion::Identity());
	//重力
	m_move.y -= 1.0f;

	//パーツのUpdate
	for (auto& part : m_parts) {
		if (part)part->Update();	
	}	
	//パーツのTRS更新
	for (auto& part : m_parts) {
		if (part)part->UpdateTRS();
	}
	//コアのTRS更新
	m_coreModel.SetPRS(m_pos, m_rot, m_scale);
	//方向ベクトル更新
	CalcDirection();
	//コアのコリジョン更新
	m_col.SetPos(m_pos); m_col.SetRot(m_rot);
	//パーツのワールド行列更新後アップデート
	for (auto& part : m_parts) {
		if (part)part->PostUTRSUpdate();
	}
}

void CDeathHotoke::PostRender() {
	//パーツの2D描画
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
	//ステータス描画
	if (!m_isDrawHUD) {
		CFont font;
		wchar_t output[256];
		swprintf_s(output, L"%.1f", m_hp);
		font.Draw(output, { 0.0f,0.25f });
	}
}

void CDeathHotoke::Damage(const ReferenceCollision& ref) {
	m_hp -= ref.damege;
}

void CDeathHotoke::PostLoopUpdate() {

	DirectX::XMFLOAT3 offset(0,0,0);
	offset.x += CMath::RandomZeroToOne()*10.0f;
	offset.y += CMath::RandomZeroToOne()*100.0f;
	offset.z += CMath::RandomZeroToOne()*1000.0f;
	for (auto& v : m_vertexBufferArray) {
		for (auto& v2 : *v) {
			v2.position.x += offset.x;
		}
	}

	int i = 0;
	m_coreModel.GetSkinModel().FindMesh([&](const auto& mesh) {
			ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();

			//頂点バッファを作成。
			{
				D3D11_MAPPED_SUBRESOURCE subresource;

				//頂点の書き込み
				HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
				if (FAILED(hr)) { return; }
				memcpy(subresource.pData, &m_vertexBufferArray[i].get()->front(), sizeof(DirectX::VertexPositionNormalTangentColorTexture) * m_vertexBufferArray[i].get()->size()); //コピー				
				deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);	

				i++;
			}		
		}
	);
}
#include "stdafx.h"
#include "CDeathHotoke.h"

#include "BP_BirdWing.h"
#include "BP_HumanLeg.h"
#include "BP_KaniArm.h"
#include "BP_FishHead.h"
#include "BP_HumanMantle.h"

void CDeathHotoke::CalcDirection() {
	m_front = CVector3::Front(); GetRot().Multiply(m_front); m_back = m_front * -1.0f;
	m_left = CVector3::Left(); GetRot().Multiply(m_left); m_right = m_left * -1.0f;
	m_up = CVector3::Up(); GetRot().Multiply(m_up); m_down = m_up * -1.0f;
}

bool CDeathHotoke::Start() {
	//�X�P�[��
	constexpr float modelScale = 0.0188f*2.0f;
	m_scale = { modelScale };

	//�R�A����
	m_coreModel.Init(L"Resource/modelData/core.cmo");	
	//�m�[�}���}�b�v�K�p
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView;	
	TextureFactory::GetInstance().Load(L"Resource/normalMap/ngasi.png", nullptr, &textureView, nullptr, true);
	m_coreModel.GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* mat) {
			mat->SetNormalTexture(textureView.Get());
			mat->SetShininess(0.9f);
			mat->SetTranslucent(0.2f);
		}
	);

	//�����蔻��
	m_col.m_collision.CreateMesh({}, {}, m_scale, m_coreModel.GetSkinModel());//TODO ���ɂ��悤��
	m_col.m_collision.SetIsHurtCollision(true);//����͋�炢����
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			if (p.EqualName(L"ReferenceCollision")) {
				//�N���X���o��
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				//�_���[�W
				Damage(*H);
			}
		}
	);
	
	//�ʒu������
	m_pos = CVector3::AxisY()*1000.0f;
	m_pos.z += 200.0f;
	
	//�p�[�c����
	m_parts[enWing] = new BP_HumanMantle(this);//BP_BirdWing
	m_parts[enLeg] = new BP_HumanLeg(this);
	m_parts[enArm] = new BP_KaniArm(this);
	m_parts[enHead] = new BP_FishHead(this);
	//�p�[�c��Start
	for (auto& part : m_parts) {
		if (part)part->Start();
	}
	
	/*
	SkinModel manto;
	manto.Init(L"Resource/modelData/manto.cmo");

	CMatrix mat, mat2;
	mat2.MakeTranslation(CVector3::Up()*1500.0f);
	mat.MakeScaling(m_scale);
	mat.Mul(mat, mat2);

	CMatrix mBias, mBiasScr;
	CoordinateSystemBias::GetBias(mBias, mBiasScr, manto.GetFBXUpAxis(), manto.GetFBXCoordinateSystem());
	mBias.Mul(mBiasScr, mBias);
	mBias.Mul(mBias, mat);

	m_stridingMeshInterface = std::make_unique<btTriangleIndexVertexArray>();
	manto.FindMesh([&](const auto& mesh) {

		ID3D11DeviceContext* deviceContext = GetEngine().GetGraphicsEngine().GetD3DDeviceContext();
		//���_�o�b�t�@���쐬�B
		{
			D3D11_MAPPED_SUBRESOURCE subresource;
			HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
			if (FAILED(hr)) {
				return;
			}
			D3D11_BUFFER_DESC bufferDesc;
			mesh->vertexBuffer->GetDesc(&bufferDesc);
			int vertexCount = bufferDesc.ByteWidth / mesh->vertexStride;
			char* pData = reinterpret_cast<char*>(subresource.pData);
			VertexBufferPtr vertexBuffer = std::make_unique<VertexBuffer>();
			CVector3 pos;
			for (int i = 0; i < vertexCount; i++) {
				pos = *reinterpret_cast<CVector3*>(pData);
				//�o�C�A�X��������B
				mBias.Mul(pos);
				vertexBuffer->push_back(pos);
				//���̒��_�ցB
				pData += mesh->vertexStride;
			}
			//���_�o�b�t�@���A�����b�N
			deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);
			m_vertexBufferArray.push_back(std::move(vertexBuffer));
		}
		//�C���f�b�N�X�o�b�t�@���쐬�B
		{
			D3D11_MAPPED_SUBRESOURCE subresource;
			//�C���f�b�N�X�o�b�t�@�����b�N�B
			HRESULT hr = deviceContext->Map(mesh->indexBuffer.Get(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &subresource);
			if (FAILED(hr)) {
				return;
			}
			D3D11_BUFFER_DESC bufferDesc;
			mesh->indexBuffer->GetDesc(&bufferDesc);
			//@todo cmo�t�@�C���̓C���f�b�N�X�o�b�t�@�̃T�C�Y��2byte�Œ�B
			IndexBufferPtr indexBuffer = std::make_unique<IndexBuffer>();
			int stride = 2;
			int indexCount = bufferDesc.ByteWidth / stride;
			unsigned short* pIndex = reinterpret_cast<unsigned short*>(subresource.pData);
			for (int i = 0; i < indexCount; i++) {
				indexBuffer->push_back(pIndex[i]);
			}
			//�C���f�b�N�X�o�b�t�@���A�����b�N�B
			deviceContext->Unmap(mesh->indexBuffer.Get(), 0);
			m_indexBufferArray.push_back(std::move(indexBuffer));
		}

		//�C���f�b�N�X���b�V�����쐬�B
		btIndexedMesh indexedMesh;
		IndexBuffer* ib = m_indexBufferArray.back().get();
		VertexBuffer* vb = m_vertexBufferArray.back().get();
		indexedMesh.m_numTriangles = (int)ib->size() / 3;
		indexedMesh.m_triangleIndexBase = (unsigned char*)(&ib->front());
		indexedMesh.m_triangleIndexStride = 12;
		indexedMesh.m_numVertices = (int)vb->size();
		indexedMesh.m_vertexBase = (unsigned char*)(&vb->front());
		indexedMesh.m_vertexStride = sizeof(CVector3);
		m_stridingMeshInterface->addIndexedMesh(indexedMesh);
		}
	);

	const btVector3 meshScaling = m_stridingMeshInterface->getScaling();

	btAlignedObjectArray<btScalar> vertices;
	btAlignedObjectArray<int> triangles;

	for (int part = 0; part < m_stridingMeshInterface->getNumSubParts(); part++)
	{
		const unsigned char * vertexbase;
		const unsigned char * indexbase;

		int indexstride;
		int stride, numverts, numtriangles;
		PHY_ScalarType type, gfxindextype;

		m_stridingMeshInterface->getLockedReadOnlyVertexIndexBase(&vertexbase, numverts, type, stride, &indexbase, indexstride, numtriangles, gfxindextype, part);

		for (int gfxindex = 0; gfxindex < numverts; gfxindex++)
		{
			float* graphicsbase = (float*)(vertexbase + gfxindex * stride);
			vertices.push_back(graphicsbase[0] * meshScaling.getX());
			vertices.push_back(graphicsbase[1] * meshScaling.getY());
			vertices.push_back(graphicsbase[2] * meshScaling.getZ());
		}

		for (int gfxindex = 0; gfxindex < numtriangles; gfxindex++)
		{
			unsigned int* tri_indices = (unsigned int*)(indexbase + gfxindex * indexstride);
			triangles.push_back(tri_indices[0]);
			triangles.push_back(tri_indices[1]);
			triangles.push_back(tri_indices[2]);
		}
	}
	*/

/*
	CMatrix mat,mat2;
	mat2.MakeTranslation(CVector3::Up()*1500.0f);
	mat.MakeScaling(m_scale);
	mat.Mul(mat,mat2);
	SkinModel manto;
	manto.Init(L"Resource/modelData/manto.cmo");
	m_meshdata.CreateFromSkinModel(manto,&mat);

	int vertex_count = (int)m_meshdata.GetVertexBuffer().size(); // �����_��
	int index_count = 0;
	for (int i = 0; i < m_meshdata.GetPolygonNum(); i++) {
		index_count += (int)m_meshdata.GetPolygonIndexs(i).size(); // ���|���S����
	}
	index_count /= 3;
	btScalar *vertices = new btScalar[vertex_count * 3]; // ���_���W���i�[����z��
	int *indices = new int[index_count * 3]; // �|���S�����\�����钸�_�ԍ����i�[����z��

	// ���_���W�̎��o��
	for (int i = 0; i < vertex_count; ++i) {
		vertices[3 * i] = m_meshdata.GetVertexBuffer()[i].x;
		vertices[3 * i + 1] = m_meshdata.GetVertexBuffer()[i].y;
		vertices[3 * i + 2] = m_meshdata.GetVertexBuffer()[i].z;
	}
	// �|���S�����\�����钸�_�ԍ��̎��o��
	int i2 = 0;
	for (int i = 0; i < m_meshdata.GetPolygonNum(); i++) {
		for (auto& ind : m_meshdata.GetPolygonIndexs(i)) {
			indices[i2] = ind;
			i2++;
		}
	}
*/

	//�\�t�g�D�{�f�[�쐬
	/*btSoftBody* soft_body = btSoftBodyHelpers::CreateFromTriMesh(*GetPhysicsWorld().GetSoftBodyWorldInfo(), &vertices[0], &triangles[0], triangles.size() / 3);
	
	//soft_body->getCollisionShape()->setMargin(0.01);
	//soft_body->generateBendingConstraints(2);
	//soft_body->setTotalMass(0.02); // �S�̂̎���
	//soft_body->m_materials[0]->m_kLST = 0.5;

	soft_body->generateBendingConstraints(2);
	soft_body->m_cfg.piterations = 2;
	soft_body->m_cfg.collisions |= btSoftBody::fCollision::VF_SS;
	soft_body->randomizeConstraints();	
	soft_body->setTotalMass(50, true);

	int nodenum = soft_body->m_nodes.size(); 
	float max_y = 0.0f;
	for (int i = 0; i < nodenum; ++i) {
		float y = soft_body->m_nodes.at(i).m_x.y();
		max_y = max(max_y, y);
		if (y > 1400.0f) {
			soft_body->setMass(i, 0.f);
		}
	}
	
	//soft_body->getCollisionShape()->setMargin(0.01);
	//soft_body->setTotalMass(10.0); // �S�̂̎���
	//soft_body->m_materials[0]->m_kLST = 0.5;
	//int nodenum = soft_body->m_nodes.size();
	//for (int i = 0; i < nodenum /2; ++i) {
	//	soft_body->setMass(i, 0.f);
	//}
	//soft_body->randomizeConstraints();
	//soft_body->setPose(true, true);

	//soft_body->m_cfg.kLF = 0.05;
	//soft_body->m_cfg.kDG = 0.01;

	GetPhysicsWorld().GetDynamicWorld()->addSoftBody(soft_body);
	
	m_soft_body = soft_body;*/

	return true;
}

void CDeathHotoke::Update() {

	/*if (GetKeyInput(VK_UP)) {
		m_soft_body->addForce(btVector3(0.01, 0.01, 0));
	}
	if (GetKeyInput(VK_DOWN)) {
		m_soft_body->addForce(btVector3(-0.01, 0.01, 0));
	}*/

	//AI���s
	if (m_ai) { m_ai->Update(); }

	//�ړ��K��
	m_pos += m_move;
	m_rot = m_rotMove * m_rot;
	//����
	m_move *= 0.5f;
	m_rotMove.Slerp(0.5f, m_rotMove, CQuaternion::Identity());
	//�d��
	m_move.y -= 1.0f;

	//�p�[�c��Update
	for (auto& part : m_parts) {
		if (part)part->Update();	
	}	
	//�p�[�c��TRS�X�V
	for (auto& part : m_parts) {
		if (part)part->UpdateTRS();
	}
	//�R�A��TRS�X�V
	m_coreModel.SetPRS(m_pos, m_rot, m_scale);
	//�����x�N�g���X�V
	CalcDirection();
	//�R�A�̃R���W�����X�V
	m_col.SetPos(m_pos); m_col.SetRot(m_rot);
	//�p�[�c�̃��[���h�s��X�V��A�b�v�f�[�g
	for (auto& part : m_parts) {
		if (part)part->PostUTRSUpdate();
	}
}

void CDeathHotoke::PostLoopUpdate() {
	for (auto& part : m_parts) {
		if (part)part->PostLoopUpdate();
	}
}

void CDeathHotoke::PostRender() {
	//�p�[�c��2D�`��
	for (auto& part : m_parts) {
		if (part)part->Draw2D();
	}
	//�X�e�[�^�X�`��
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

//�R�A�̒��_����e�X�g
/*void CDeathHotoke::PostLoopUpdate() {

	DirectX::XMFLOAT3 offset(0,0,0);
	offset.x += CMath::RandomZeroToOne()*10.0f;
	offset.y += CMath::RandomZeroToOne()*100.0f;
	offset.z += CMath::RandomZeroToOne()*1000.0f;
	for (auto& v : m_vertexPositionArray) {
		for (auto& v2 : *v) {
			v2.position.x += offset.x;
		}
	}

	int i = 0;
	m_coreModel.GetSkinModel().FindMesh([&](const auto& mesh) {
			ID3D11DeviceContext* deviceContext = GetGraphicsEngine().GetD3DDeviceContext();

			//���_�o�b�t�@���쐬�B
			{
				D3D11_MAPPED_SUBRESOURCE subresource;

				//���_�̏�������
				HRESULT hr = deviceContext->Map(mesh->vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
				if (FAILED(hr)) { return; }
				memcpy(subresource.pData, &m_vertexPositionArray[i].get()->front(), sizeof(DirectX::VertexPositionNormalTangentColorTexture) * m_vertexPositionArray[i].get()->size()); //�R�s�[				
				deviceContext->Unmap(mesh->vertexBuffer.Get(), 0);	

				i++;
			}		
		}
	);
}*/
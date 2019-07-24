#include "stdafx.h"
#include "TreeGene.h"
#include "CDeathHotoke.h"

/// <summary>
/// 木
/// </summary>
int Tree::m_sInstancingMax = 512;

void Tree::Init(const CVector3& pos, const CVector3& normal){
	m_pos = pos;

	//バリエーション
	float sizeScale = 0.8f*CMath::RandomZeroToOne() > 0.5f ? 1.0f : 1.5f;
	float radY = -CMath::PI2 + CMath::PI2*2.0f*CMath::RandomZeroToOne();
	m_rot.SetRotation(CVector3::AxisY(), radY);

	//近景モデル
	//if (CMath::RandomZeroToOne() > 0.8f) {
	//	m_model.Init(m_sInstancingMax, L"Resource/modelData/tree_notall.cmo");
		//茂み
		//m_model.Init(m_sInstancingMax, L"Resource/modelData/sigemi.cmo", nullptr, 0, enFbxUpAxisY);
		//sizeScale *= 0.05f;
	//}
	//else {
		m_model.Init(m_sInstancingMax, L"Resource/modelData/tree_tall.cmo");
	//}
	m_model.SetPos(m_pos);
	m_model.SetRot(m_rot);
	m_model.SetScale(sizeScale);
	m_model.SetIsDraw(true);
	m_model.GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);
	m_model.GetInstancingModel()->GetModelRender().GetSkinModel().FindMaterialSetting(
		[&](MaterialSetting* me) {
			if (me->EqualMaterialName(L"leaf fern")) {
				me->SetIsUseTexZShader(true);
			}
		}
	);
	//モデルの高さ取得
	CVector3 min, max;
	m_model.GetInstancingModel()->GetModelRender().GetSkinModel().GetBoundingBox(min, max);
	if (m_model.GetInstancingModel()->GetModelRender().GetSkinModel().GetFBXUpAxis() == enFbxUpAxisZ) {
		m_modelHeight = max.z;//Z-UP
		m_modelRadius = (abs(max.x) + abs(max.y) + abs(min.x) + abs(min.y)) / 4.0f;
	}
	else {
		m_modelHeight = max.y;//Y-UP
		m_modelRadius = (abs(max.x) + abs(max.z) + abs(min.x) + abs(min.z)) / 4.0f;
	}

	//遠景モデル
	//if (m_model.GetInstancingModel()->GetModelRender().GetSkinModel().EqualModelName(L"tree_notall")) {
	//	m_imposter.Init(L"Resource/modelData/tree_notall.cmo", { 2048 * 4, 2048 * 4 }, { 69,35 }, m_sInstancingMax);
	//}
	//else {
		m_imposter.Init(L"Resource/modelData/tree_tall.cmo", { 2048 * 4, 2048 * 4 }, { 69,35 }, m_sInstancingMax);
	//}
	m_imposter.SetPos(m_pos);
	m_imposter.SetRotY(radY);
	m_imposter.SetScale(sizeScale);
	m_imposter.SetIsDraw(false);
	m_imposter.SetIsShadowCaster(false);

	//当たり判定
	constexpr float radius = 50.0f;
	m_col.m_collision.CreateSphere(m_pos + CVector3::AxisY()*radius*sizeScale, {}, radius*sizeScale);
	m_col.m_reference.position = m_pos + CVector3::AxisY()*radius*sizeScale;
	m_col.m_collision.SetIsHurtCollision(true);
	m_col.m_collision.SetCallback(
		[&](SuicideObj::CCollisionObj::SCallbackParam& p) {
			//なぎ倒される
			if (m_isHited) { return; }//すでに倒れてる
			if (p.EqualName(L"ReferenceCollision")) {
				//クラス取り出す
				ReferenceCollision* H = p.GetClass<ReferenceCollision>();
				//倒れる方向算出
				CVector3 dir = m_pos - H->position; dir.y = 0.0f; 
				if (dir.LengthSq() > FLT_EPSILON) {
					dir.Normalize();
					float rad = CVector3::AngleOf2NormalizeVector(dir, CVector3::AxisZ());
					float sign = 1.0f; if (CVector2(dir.x,dir.z).Cross({ 0.0f, 1.0f }) < 0.0f) { sign = -1.0f; }
					m_rotOffset = CQuaternion(CVector3::AxisY(), sign*rad) * CQuaternion(CVector3::AxisX(), CMath::DegToRad(80.0f));
					
					//レイで判定
					CVector3 topPos = m_pos+CVector3::AxisY()*(m_modelHeight * m_model.GetScale().y);
					btVector3 rayStart = topPos;
					CVector3 topPosRotaed = topPos;
					m_rotOffset.Multiply(topPosRotaed);
					btVector3 rayEnd = topPosRotaed + CVector3::AxisY()*(topPosRotaed.y - topPos.y);
					btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
					GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
					if (gnd_ray.hasHit()) {
						CVector3 dir2 = (gnd_ray.m_hitPointWorld + CVector3::AxisY()*m_modelRadius) - m_pos; dir2.Normalize();
						//角度
						float rad = CVector3::AngleOf2NormalizeVector(CVector3::AxisY(), dir2);
						if (rad > FLT_EPSILON) {
							//回転軸
							CVector3 rotationAxis;
							rotationAxis.Cross(CVector3::AxisY(), dir2);
							rotationAxis.Normalize();
							m_rotOffset.SetRotation(rotationAxis, rad);
						}
						else {
							return;
						}
					}
					//設定		
					m_model.SetRot(m_rotOffset*m_rot);
					m_isHited = true;
				}
			}
		}
	);
	//m_col.m_collision.SetEnable(false);
	//m_col.IGameObject::SetEnable(false);
}

void Tree::PostLoopUpdate() {
	constexpr float nearDistance = CMath::Square(1200.0f*2.0f), farDistance = CMath::Square(2500.0f);
	const float distance = (m_pos - GetMainCamera()->GetPos()).LengthSq();

	if (GetKeyInput(VK_TAB) || distance < nearDistance) {
		if (m_model.GetIsDraw() == false) {
			m_model.SetIsDraw(true);
			m_imposter.SetIsDraw(false);
			m_col.m_collision.SetEnable(true);
		}
	}
	else {
		if (m_model.GetIsDraw() == true) {
			m_model.SetIsDraw(false);
			m_imposter.SetIsDraw(true);
			m_col.m_collision.SetEnable(false);//遠いと判定も無効化
			m_isHited = false;
			m_rotOffset = CQuaternion::Identity();
			m_model.SetRot(m_rotOffset*m_rot);
		}
	}
}

/*
/// <summary>
/// ツリージェネレーター
/// </summary>
void TreeGene::Generate(const CVector3& minArea, const CVector3& maxArea, int num) {

	//インスタンシングの数設定
	if (Tree::m_sInstancingMax < num) {
		Tree::m_sInstancingMax = num;
	}

	//木々を作る
	for (int i = 0; i < num; i++) {
		//座標生成
		CVector3 pos = { minArea.x + (maxArea.x - minArea.x)*CMath::RandomZeroToOne(), maxArea.y, minArea.z + (maxArea.z - minArea.z)*CMath::RandomZeroToOne() };
		//回転生成
		CQuaternion rot(CVector3::AxisY(), CMath::PI2*CMath::RandomZeroToOne());
		//レイで判定
		btVector3 rayStart = pos;
		btVector3 rayEnd = btVector3(pos.x, minArea.y, pos.z);
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			//接触点を座標に
			pos = gnd_ray.m_hitPointWorld;
		}		
		//生まれろ!
		m_trees.emplace_back(std::make_unique<Tree>(i,pos));
	}
}

void TreeGene::Clear() {
	m_trees.clear();
}
*/

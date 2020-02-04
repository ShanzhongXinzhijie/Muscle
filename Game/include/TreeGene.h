#pragma once

#include"IFu.h"
#include"LOD.h"
#include"StageObjectGenerator.h"

/// <summary>
/// 人間!
/// </summary>
class CHuman : public IGameObject {
public:	
	CHuman() {
		SetName(L"CHuman");
	}

	/// <summary>
	/// 開始時処理
	/// </summary>
	bool Start()override {
		m_anim.Load(L"Resource/animation/human/stand.tka");
		m_human.Init(L"Resource/modelData/human.cmo", &m_anim, 1);
		m_human.SetScale(10.0f);

		//レイで判定
		btVector3 rayStart = btVector3(0.0f, 100000.0f, 0.0f);
		btVector3 rayEnd = btVector3(0.0f, -100000.0f, 0.0f);
		btCollisionWorld::ClosestRayResultCallback gnd_ray(rayStart, rayEnd);
		GetEngine().GetPhysicsWorld().RayTest(rayStart, rayEnd, gnd_ray);
		if (gnd_ray.hasHit()) {
			//接触点を座標に
			m_human.SetPos(gnd_ray.m_hitPointWorld);
		}

		//m_humanCam.SetViewAngleDeg(25.0f);
		m_humanCam.SetFar(150000.0f);

		return true;
	}

	/// <summary>
	/// この人間の視点をメインカメラに設定
	/// </summary>
	/// <param name="targetView">注視点</param>
	/// <param name="cameraNum">セット先のカメラ番号</param>
	void EnableHumanCamera(const CVector3& targetView, int cameraNum = 0) {
		m_humanCam.SetPos(m_human.GetBonePos(m_human.FindBoneID(L"Head")) + CVector3::AxisY() * 0);
		m_humanCam.SetTarget(targetView);
		SetMainCamera(&m_humanCam);
		SetCameraToList(cameraNum, &m_humanCam);
		m_human.SetIsDraw(false);
	}
	/// <summary>
	/// メインカメラから人間視点を外すときに呼ぶ処理
	/// ※新たなカメラの設定は別でやってください
	/// </summary>
	void DisableHumanCamera() {
		m_human.SetIsDraw(true);
	}

private:
	AnimationClip m_anim;
	GameObj::CSkinModelRender m_human;
	GameObj::PerspectiveCamera m_humanCam;
};

/// <summary>
/// ヘリコプター
/// </summary>
class CHelicopter : public IStageObject, public IFu {
public:
	using IStageObject::IStageObject;

	//初期化関数
	void Init(const CVector3& pos, const CVector3& normal)override;

private:
	//グラフィック
	AnimationClip m_animHeri;
	GameObj::CInstancingModelRender m_heri;
	
public:
	static inline int m_sInstancingMax = 32; //このクラスの最大インスタンス数
};

/// <summary>
/// 鉄塔
/// </summary>
class TransmissionTower : public IStageObject, public IGameObject, public IFu {
public:	
	using IStageObject::IStageObject;

	//初期化関数
	void Init(const CVector3& pos, const CVector3& normal)override;

	//モデルを取得
	GameObj::CSkinModelRender& GetModel() {
		return m_model;
	}

	//ワイヤーを通されているか?
	void SetIsWired(bool b) { m_isWired = b; }
	bool IsWired()const { return m_isWired; }

private:
	//グラフィック
	GameObj::CSkinModelRender m_model;
	std::unique_ptr<GameObj::CSkinModelRender> m_wire[6];
	//std::unique_ptr<GameObj::CSkinModelRender> m_dodai[4];

	bool m_isWired = false;//ワイヤー通されてるか?
};

/// <summary>
/// 草
/// </summary>
class Grass : public IGameObject {
public:
	//開始時処理
	bool Start()override;
	//動作ループ後処理
	void PostLoopUpdate()override;
	//描画前処理
	void Pre3DRender(int)override;

	//どのカメラに表示するか設定
	void SetDrawCameraNum(int camnum) {
		m_cameraNum = camnum;
	}

private:
	//位置を再設定
	void RePos(GameObj::ICamera* mainCamera);

private:
	//グラフィック
	GameObj::CInstancingModelRender m_model;

	int m_cameraNum = 0;//どのカメラに表示するか

public:
	static inline constexpr int m_sInstancingMax = 512 * 4 * PLAYER_NUM; //このクラスの最大インスタンス数
};

/// <summary>
/// 木でーす
/// </summary>
class Tree : public IStageObject{
public:
	using IStageObject::IStageObject;

	//初期化関数
	void Init(const CVector3& pos, const CVector3& normal)override;

private:
	//グラフィック
	LODSwitcher m_lodSwitcher;
	LODInstancingModel m_model;
	LODImposter m_imposter;
	LODNothing m_noDraw;

	//座標とか
	CVector3 m_pos;
	CQuaternion m_rot, m_rotOffset;

	//当たり判定
	//DHCollision m_col;
	//bool m_isHited = false;
	//float m_modelHeight = 100.0f, m_modelRadius = 0.0f;

public:
	static inline int m_sInstancingMax = 512; //このクラスの最大インスタンス数
};

class dammy : public IStageObject {
public:
	using IStageObject::IStageObject;
	void Init(const CVector3& pos, const CVector3& normal)override {
		//LOD初期化
		CVector2 FrustumSize; GetMainCamera()->GetFrustumPlaneSize(2400.0f / 3.0f, FrustumSize);//TODO 木のScaleに連動
		m_lodSwitcher.AddDrawObject(&m_noDraw[2], FrustumSize.y);
		m_lodSwitcher.AddDrawObject(&m_noDraw[1], FrustumSize.y * 3.0f);
		m_lodSwitcher.AddDrawObject(&m_noDraw[0]);

		//m_model.Get().Init(Tree::m_sInstancingMax, L"Resource/modelData/realTree_S.cmo");
		//m_model.Get().SetPos(pos);
		//m_model.Get().SetRot({});
		//m_model.Get().SetScale(1.0f);
		//m_model.Get().SetIsDraw(false);
		//m_model.Get().GetInstancingModel()->GetModelRender().SetIsShadowCaster(false);

		//遠景モデル
		//if (!m_imposter.Get().Init( L"Resource/modelData/realTree_S.cmo", Tree::m_sInstancingMax)) {
		//	//初回ロード
		//	SkinModel model;
		//	model.Init(L"Resource/modelData/realTree_S.cmo");
		//	m_imposter.Get().Init(L"Resource/modelData/realTree_S.cmo", model, { 2, 2 }, { 1,1 }, Tree::m_sInstancingMax);
		//}
		/*m_imposter.Get().SetPos(pos);
		m_imposter.Get().SetScale(1.0f);
		m_imposter.Get().SetIsDraw(false);
		m_imposter.Get().SetIsShadowCaster(false);*/
	}

	LODSwitcher m_lodSwitcher;
	//LODInstancingModel m_model; LODImposter m_imposter; 
	LODNothing m_noDraw[3];
};
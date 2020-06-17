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
class Grass {
public:
	//コンストラクタ
	Grass() :m_model(false) {}

	//開始時処理
	bool Start();
	//動作ループ後処理
	void PostLoopUpdate();
	//描画前処理
	void Pre3DRender(int);

	//どのカメラに表示するか設定
	void SetDrawCameraNum(int camnum) {
		m_cameraNum = camnum;
	}

	//表示設定
	void DisableDraw() {
		m_model.SetIsDraw(false);
	}

private:
	//位置を再設定
	void RePos(GameObj::ICamera* mainCamera);

private:
	//グラフィック
	GameObj::CInstancingModelRender m_model;//登録しない

	int m_cameraNum = 0;//どのカメラに表示するか

public:
	static inline constexpr int m_sInstancingMax = 512 * 4 * PLAYER_NUM; //このクラスの最大インスタンス数
};

/// <summary>
/// Grassクラスを実行するクラス
/// </summary>
class GrassRunner : public IGameObject {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GrassRunner() {
		//表示するカメラ設定
		int i = 0, playerNum = 0;
		for (auto& grass : m_grass) {
			if (i > Grass::m_sInstancingMax / PLAYER_NUM * (playerNum+1)) {
				playerNum++;
			}
			grass.SetDrawCameraNum(playerNum);
			i++;
		}
	}

	/// <summary>
	/// 初期化
	/// </summary>
	void Init() {
		m_isEnable = true;
		int i = 0;
		for (auto& grass : m_grass) {
			if (i <= Grass::m_sInstancingMax / PLAYER_NUM * ViewCameraList().size()) {
				grass.Start();
			}
			else {
				m_enableGrassNum = i; 
				break;
			}
			i++;
		}
	}
	/// <summary>
	/// 無効化
	/// </summary>
	void Disable() {
		m_isEnable = false;
		for (auto& grass : m_grass) {
			grass.DisableDraw();
		}
		m_enableGrassNum = 0;
	}
	
	/// <summary>
	/// 動作ループ後処理
	/// </summary>
	void PostLoopUpdate()override {
		if (!m_isEnable) { 
			return;
		}
		int i = 0;
		for (auto& grass : m_grass) {
			if (i >= m_enableGrassNum) {
				break;
			}
			grass.PostLoopUpdate();
			i++;
		}
	}
	
	/// <summary>
	/// 描画前処理
	/// </summary>
	void Pre3DRender(int screenNum)override {
		if (!m_isEnable) {
			return;
		}
		int i = 0;
		for (auto& grass : m_grass) {
			if (i >= m_enableGrassNum) {
				break;
			}
			grass.Pre3DRender(screenNum);
			i++;
		}
	}

private:
	bool m_isEnable = false;
	//草
	int m_enableGrassNum = 0;
	Grass m_grass[Grass::m_sInstancingMax];
};

/// <summary>
/// 木でーす
/// </summary>
class Tree : public IStageObject{
public:
	Tree(): m_lodSwitcher(false), m_model(false), m_imposter(false){}

	//初期化関数
	void Init(const CVector3& pos, const CVector3& normal)override;

	/// <summary>
	/// 無効化
	/// </summary>
	void Disable() {
		m_model.SetIsDraw(false);
		m_imposter.SetIsDraw(false);
	}

	//Update
	void PreLoopUpdate() {
		m_lodSwitcher.PreLoopUpdate();
	}
	void PostLoopPostUpdate() {
		m_model.Get().PostLoopPostUpdate();
		m_imposter.Get().GetInstancingModel().PostLoopPostUpdate();
	}
	void Pre3DRender(int num) {
		m_lodSwitcher.Pre3DRender(num);
	}

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
	static inline constexpr int m_sInstancingMax = 4000 + 1000;//このクラスの最大インスタンス数
};


/// <summary>
/// Treeクラスを実行するクラス
/// </summary>
class TreeRunner : public IGameObject {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Init(StageObjectGenerator& objGene);
	/// <summary>
	/// 無効化
	/// </summary>
	void Disable() {
		m_isEnable = false;
		for (auto& tree : m_tree) {
			tree.Disable();
		}
		m_enableTreeNum = 0;
	}

	/// <summary>
	/// 動作ループ前処理
	/// </summary>
	void PreLoopUpdate()override {
		if (!m_isEnable) {
			return;
		}
		int i = 0;
		for (auto& tree : m_tree) {
			if (i >= m_enableTreeNum) {
				break;
			}
			tree.PreLoopUpdate();
			i++;
		}
	}

	/// <summary>
	/// 動作ループ後処理
	/// </summary>
	void PostLoopPostUpdate()override {
		if (!m_isEnable) {
			return;
		}
		int i = 0;
		for (auto& tree : m_tree) {
			if (i >= m_enableTreeNum) {
				break;
			}
			tree.PostLoopPostUpdate();
			i++;
		}
	}

	/// <summary>
	/// 描画前処理
	/// </summary>
	void Pre3DRender(int screenNum)override {
		if (!m_isEnable) {
			return;
		}
		int i = 0;
		for (auto& tree : m_tree) {
			if (i >= m_enableTreeNum) {
				break;
			}
			tree.Pre3DRender(screenNum);
			i++;
		}
	}

private:
	bool m_isEnable = false;
	//木
	int m_enableTreeNum = 0;
	Tree m_tree[Tree::m_sInstancingMax];
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

static inline GrassRunner g_grassRunner;
static inline TreeRunner g_treeRunner;